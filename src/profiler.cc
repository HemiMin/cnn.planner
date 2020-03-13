#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <fstream>

#include "general/data_type.h"
#include "parameter/profiler_parser.h"
#include "parameter/profiler_parameter.h"
#include "arch/architecture.h"
#include "loop/cnn_loop.h"
#include "analysis/analysis_report.h"
#include "statistic/csv_writer.h"

using std::cout;
using std::endl;
using std::unique_ptr;
using std::ifstream;

using parameter::ProfilerParser;
using arch::Architecture;
using loop::CnnLoop;
using analysis::AnalysisReport;
using statistic::CsvWriter;

// Initialize global variables.
int kStride     = NON_VALID;
int kFilter_len = NON_VALID;
int kPadding    = NON_VALID;

//TODO (MinsuKim): Add more logging here.
int main(int argc, char** argv)
{
  google::InitGoogleLogging(argv[0]);
  google::SetLogDestination(google::GLOG_INFO, "log/log.");

  cout << "[Back-end][Profiler] Build profiler parameter..." << endl;
  unique_ptr<ProfilerParser> parser(new ProfilerParser());
  ProfilerParameter* param = parser->BuildParameter(argc, argv);
  cout << "[Back-end][Profiler] Success to build parameter!" << endl;

  cout  << "---------------------------------------------------------" <<endl
        << "                     Profiling start..."                   <<endl
        << "---------------------------------------------------------" <<endl;
  
  unique_ptr<Architecture> arch(new Architecture(*param));
  unique_ptr<CnnLoop> loop(new CnnLoop(*param));

  Variables on_loop, parl_loop;
  Structure *on_strt = new Structure(), *off_strt = new Structure();
  cout<< "[Back-end][Profiler] Read tiling and structure from dump files"<<endl;
  ifstream varset_dump(param->GetTilingDumpFile());
  ifstream strt_dump(param->GetLoopSequenceDumpFile());
  varset_dump >> on_loop >> parl_loop;
  strt_dump >> *off_strt >> *on_strt;
  VariableSet* varset = new VariableSet(loop->GetVariableSet());
  varset->SetOnLoopVariables(on_loop);
  varset->SetParlLoopVariables(parl_loop);
  off_strt->TagStationary(varset->GetOffLoopVariables(), 
                          varset->GetOnLoopVariables());
  loop->SetVariableSet(varset);
  loop->SetOffStructure(off_strt);
  loop->SetOnStructure(on_strt);
  varset_dump.close();
  strt_dump.close();
  /* #region Logging */
  LOG(INFO) << "Tiling Factors.";
  LOG(INFO) << "  TIW: " << loop->GetVariableSet().GetTiw();
  LOG(INFO) << "  TIH: " << loop->GetVariableSet().GetTih();
  LOG(INFO) << "  TIC: " << loop->GetVariableSet().GetTic();
  LOG(INFO) << "  TKW: " << loop->GetVariableSet().GetTkw();
  LOG(INFO) << "  TKH: " << loop->GetVariableSet().GetTkh();
  LOG(INFO) << "  TOW: " << loop->GetVariableSet().GetTow();
  LOG(INFO) << "  TOH: " << loop->GetVariableSet().GetToh();
  LOG(INFO) << "  TOC: " << loop->GetVariableSet().GetToc();
  /* #endregion */
  cout << "[Back-end][Profiler] Analyzing..." << endl;
  unique_ptr<AnalysisReport> report(new AnalysisReport());
  report->PreAnalyze( loop->GetVariableSet(),
                      loop->GetOffStructure(),
                      loop->GetOnStructure(),
                      *arch);
  long int latency;
  ifstream latency_file(param->GetLatencyFile());
  latency_file >> latency;
  latency_file.close();
  
  report->Analyze(loop->GetVariableSet(), latency, *arch);
  std::cerr << "[Back-end][Profiler] Psum variable: " << std::hex 
            << report->GetEncodedPsumVariables()      << std::dec
            << endl;

  if (param->IsVerbose()) {
    cout  << "---------------------------------------------------------" <<endl
          << "| Loop Variables                                         |"<<endl  
          << "---------------------------------------------------------" <<endl
          << loop->GetVariableSet()
          << "---------------------------------------------------------" <<endl
          << "| Loop Structures                                        |"<<endl
          << "---------------------------------------------------------" <<endl
          << "[ off-chip :::: on-chip ]"                                 <<endl
          << loop->GetOffStructure()
          << "[ on-chip  :::: register file ]"                           <<endl
          << loop->GetOnStructure()                                      <<endl;
    cout  << "---------------------------------------------------------" <<endl;
    report->PrintComputeAnalysisReport(cout);
    cout  << "---------------------------------------------------------" <<endl;
    report->PrintDataReuseAnalysisReport(cout);
    cout  << "---------------------------------------------------------" <<endl;
    report->PrintOnChipAccessAnalysisReport(cout);
    cout  << "---------------------------------------------------------" <<endl;
    report->PrintOffChipAccessAnalysisReport(cout);
  }
  cout  << "---------------------------------------------------------" <<endl;
  cout  << "| Stationary                                             |"<<endl;
  cout  << "---------------------------------------------------------" <<endl;
  if (loop->GetOffStructure().IsInputStationary())
    cout<< "INPUT STATIONARY"  << endl;
  if (loop->GetOffStructure().IsWeightStationary())
    cout<< "WEIGHT STATIONARY" << endl;
  if (loop->GetOffStructure().IsOutputStationary())
    cout<< "OUTPUT STATIONARY" << endl;
  cout  << "---------------------------------------------------------" <<endl;
  report->PrintRooflineAnalysisReport(cout);
  cout  << "---------------------------------------------------------" <<endl;
  report->PrintEnergyAnalysisReport(cout);
  cout  << "---------------------------------------------------------" <<endl;

  cout  << "[Back-end][Profiler] Dump report file to " << param->GetReportFile()
        << endl;
  unique_ptr<CsvObject> csv_obj(new CsvObject(param->GetLayerName(),
                                              loop->GetVariableSet(),
                                              loop->GetOffStructure(),
                                              *report));
  unique_ptr<CsvWriter> csv_writer(new CsvWriter());
  csv_writer->WriteCsv(param->GetReportFile(), *csv_obj);
  cout << "[Back-end][Profiler] Success to dump reporting" << endl;

  return EXIT_SUCCESS;
}