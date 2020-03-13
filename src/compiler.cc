#include <glog/logging.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <memory>

#include "general/data_type.h"
#include "parameter/compiler_parser.h"
#include "loop/cnn_loop.h"
#include "arch/architecture.h"
#include "loop/scheduler.h"
#include "codegen/simulation_code_generator.h"
#include "codegen/ir_gaia/ir_gaia.h"

using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::unique_ptr;

using parameter::CompilerParser;
using loop::CnnLoop;
using arch::Architecture;
using loop::Scheduler;
using codegen::CodeGenerator;
using codegen::simulation::SimulationCodeGenerator;
using codegen::gaia::GaiaIr;

// Initialize global variables.
int kStride     = NON_VALID;
int kFilter_len = NON_VALID;
int kPadding    = NON_VALID;

int main(int argc, char** argv)
{
  google::InitGoogleLogging(argv[0]);
  google::SetLogDestination(google::GLOG_INFO, "log/log.");
  //google::LogToStderr();  //NOTE Activate this code if you want to see all logs on your terminal.

  cout << "[Back-end][Compiler] Build compiler parameter..." << endl;
  unique_ptr<CompilerParser> parser(new CompilerParser());
  CompilerParameter* param = parser->BuildParameter(argc, argv); // Don't use unique_ptr here because of polymorphism.
  cout << "[Back-end][Compiler] Success to build parameter!" << endl;

  cout  << "---------------------------------------------------------" <<endl
        << "                   Compilation start..."                   <<endl
        << "---------------------------------------------------------" <<endl;

  unique_ptr<Architecture> arch(new Architecture(*param));
  unique_ptr<CnnLoop> loop(new CnnLoop(*param));
  unique_ptr<Scheduler> sched(new Scheduler());

  if (!param->GetPreScheduled()) {
    loop.reset(sched->SearchBestLoopCase(*loop, *arch));
    /* #region Off-chip structure Tagging */
    Structure* tagged_off_strt = new Structure(loop->GetOffStructure());
    tagged_off_strt->TagStationary( loop->GetVariableSet().GetOffLoopVariables(), 
                                    loop->GetVariableSet().GetOnLoopVariables());
    Structure* on_strt = new Structure(loop->GetOnStructure());
    on_strt->SetOutputStationary();
    loop->SetOffStructure(tagged_off_strt);
    loop->SetOnStructure(on_strt);
    /* #endregion */
    loop->SetOnStructure(sched->FixOnLoopStructure());
    loop->MoveFullyTiledToInnerMost();
    loop->CheckValid();
    /* #region Logging */
    LOG(INFO) << "Final Tiling Factors.";
    LOG(INFO) << "  TIW: " << loop->GetVariableSet().GetTiw();
    LOG(INFO) << "  TIH: " << loop->GetVariableSet().GetTih();
    LOG(INFO) << "  TIC: " << loop->GetVariableSet().GetTic();
    LOG(INFO) << "  TKW: " << loop->GetVariableSet().GetTkw();
    LOG(INFO) << "  TKH: " << loop->GetVariableSet().GetTkh();
    LOG(INFO) << "  TOW: " << loop->GetVariableSet().GetTow();
    LOG(INFO) << "  TOH: " << loop->GetVariableSet().GetToh();
    LOG(INFO) << "  TOC: " << loop->GetVariableSet().GetToc();
    /* #endregion */
    cout << "[Back-end][Compiler] Dump tiling information..." << endl;
    ofstream varset_dump(param->GetTilingDumpFile());
    ofstream strt_dump(param->GetLoopSequenceDumpFile(), std::ios::app);
    varset_dump << loop->GetVariableSet().GetOnLoopVariables() << endl
                << loop->GetVariableSet().GetParlLoopVariables();
    strt_dump << loop->GetOffStructure() << endl
              << loop->GetOnStructure();

    varset_dump.close();
    strt_dump.close();
  } else {
    Variables on_loop, parl_loop;
    Structure *on_strt=new Structure(), *off_strt=new Structure();
    cout  << "[Back-end][Compiler] Read tiling and structure from pre-scheduled file"
          << endl;
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
    LOG(INFO) << "Final Tiling Factors.";
    LOG(INFO) << "  TIW: " << loop->GetVariableSet().GetTiw();
    LOG(INFO) << "  TIH: " << loop->GetVariableSet().GetTih();
    LOG(INFO) << "  TIC: " << loop->GetVariableSet().GetTic();
    LOG(INFO) << "  TKW: " << loop->GetVariableSet().GetTkw();
    LOG(INFO) << "  TKH: " << loop->GetVariableSet().GetTkh();
    LOG(INFO) << "  TOW: " << loop->GetVariableSet().GetTow();
    LOG(INFO) << "  TOH: " << loop->GetVariableSet().GetToh();
    LOG(INFO) << "  TOC: " << loop->GetVariableSet().GetToc();
    /* #endregion */
  }
  cout << "[Back-end][Compiler] Code generation start..." << endl;
  // Don't use unique_ptr here because of polymorphism.
  CodeGenerator* code_gen = new SimulationCodeGenerator(
                                                      param->GetCodeFile(),
                                                      param->GetLatencyFile(),
                                                      param->GetTimestampFile()
                                                      );
  code_gen->GenCode(*loop, *arch);
  // Generate Gaia IR.
  cout << "[Back-end][Compiler] Gaia IR generation start..." << endl;
  codegen::LayerList layer_list = {codegen::LayerType::CONV};
  GaiaIr* gaia_ir = new GaiaIr(layer_list, *loop, *arch);

  cout << "[Back-end][Compiler] Write Gaia IR to "<<param->GetGaiaFile()<< endl;

  ofstream gaia_code_f(param->GetGaiaFile());
  gaia_code_f << *gaia_ir;
  if (strcmp(param->GetGaiaFile(), "conv_3_1.gaia")==0)
    std::cout << *gaia_ir;
  gaia_code_f.close();
  cout << "[Back-end][Compiler] Finish Compile..." << endl;

  delete param;
  delete gaia_ir;
  delete code_gen;

  return EXIT_SUCCESS;
}
