#include "planner/cnn_planner.h"

#include <stdlib.h>
#include <string.h>
#include <glog/logging.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "general/data_type.h"
#include "general/loop_structure.h"
#include "parameter/parameter_parser.h"
#include "arch/architecture.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::ceil;
using std::max;

using parameter::ParameterParser;
using general::LoopStructure;
using arch::Architecture;

planner::CnnPlanner::CnnPlanner(const Parameter& param)
{
  /* #region Logging */
  LOG(INFO) << "Generate CNN planner object." << endl;
  /* #endregion */
  PrintConnection();

  InitLocalVariables(param);
  InitLocalObjects(param);

  architecture_.CheckVariables();
  variable_set_.CheckValid();
  CheckTileSizeWithMemSize();
}

void planner::CnnPlanner::InitLocalVariables(const Parameter& param)
{
  architecture_ = Architecture(param);

  strncpy(sim_code_file_, param.GetSimulationCodeFile(), STR_LEN);
  strncpy(latency_file_, param.GetLatencyFile(), STR_LEN);
  strncpy(timestamp_file_, param.GetTimestampFile(), STR_LEN);
  strncpy(testcase_name_, param.GetTestcaseName(), STR_LEN);
  strncpy(tiling_factor_dump_file_, param.GetTilingFactorDumpFile(), STR_LEN);
  strncpy(loop_structure_dump_file_, param.GetLoopStructureDumpFile(),
    STR_LEN);
  /* #region Logging */
  LOG(INFO) << "Initialize architecture configurations.";
  LOG(INFO) << "  MAC cycles: " << architecture_.GetMacCycles() << " cycles";
  LOG(INFO) << "  Bandwidth: " << architecture_.GetBandwidth() << " GB/s";
  LOG(INFO) << "  Frequency: " << architecture_.GetFrequency() << " GHz";
  LOG(INFO) << "  On-chip energy: " << architecture_.GetOnChipEnergy() << " nJ";
  LOG(INFO) << "  Off-chip energy: " << architecture_.GetOffChipEnergy() 
    << " nJ";
  LOG(INFO) << "  Input memory size: " << architecture_.GetInputMemSize()
    << " Bytes";
  LOG(INFO) << "  Weight memory size: " << architecture_.GetWeightMemSize()
    << " Bytes";
  LOG(INFO) << "  Output memory size: " << architecture_.GetOutputMemSize()
    << " Bytes";
  LOG(INFO) << "  PE dimension";
  for (auto pe_dim : architecture_.GetPeDim()) {
    LOG(INFO) << "    (" << pe_dim[0] << ", " << pe_dim[1] << ")";
  }
  LOG(INFO) << "  PE structure";
  vector<vector<DataDimension>> pe_structure = architecture_.GetPeStructure();
  LOG(INFO) << "    <<< Row >>>";
  for (auto pe_row : pe_structure[0]) {
    LOG(INFO) << "      " << pe_row;
  }
  LOG(INFO) << "    <<< Column >>>";
  for (auto pe_col : pe_structure[1]) {
    LOG(INFO) << "      " << pe_col;
  }
  LOG(INFO) << "Initialize local variables.";
  LOG(INFO) << "  Simluation source code: " << sim_code_file_;
  LOG(INFO) << "  Latency file: " << latency_file_;
  LOG(INFO) << "  Timestamp file: " << timestamp_file_;
  LOG(INFO) << "  Testcase name: " << testcase_name_;
  LOG(INFO) << "  Tiling factor dump file: " << tiling_factor_dump_file_;
  LOG(INFO) << "  Loop structure dump file: " << loop_structure_dump_file_;
  /* #endregion */
}

void planner::CnnPlanner::InitLocalObjects(const Parameter& param)
{
  /* #region Logging */
  LOG(INFO) << "Initialize local objects." << endl;
  /* #endregion */
  //TODO (MinsuKim): Lazy allocate output length here.
  variable_set_ = LoopVariableSet(param);
#ifndef MANUAL
  LoopVariables best_tiling_case;
  scheduler_.SearchBestLoopCase(variable_set_.GetInterLoopVariables(),
    architecture_, &best_tiling_case, &inter_loop_structure_);
  variable_set_.SetIntraLoopVariables(best_tiling_case);
#else
  variable_set_.SetIntraLoopVariables(ParseTilingFactor());
  inter_loop_structure_ = LoopStructure(ParseInterLoopSequence());
#endif
  inter_loop_structure_.TagStationary(variable_set_.GetInterLoopVariables(),
    variable_set_.GetIntraLoopVariables());
  intra_loop_structure_ = scheduler_.FixIntraLoopStructure();
  variable_set_.SetUnrollLoopVariables(scheduler_.MakeUnrollLoopVariables(
    variable_set_.GetIntraLoopVariables(), architecture_));
  /* #region Logging */
  LOG(INFO) << "Unroll loop variables";
  LOG(INFO) << "  IW: " << variable_set_.GetUnrollLoopVariables().GetIw();
  LOG(INFO) << "  IH: " << variable_set_.GetUnrollLoopVariables().GetIh();
  LOG(INFO) << "  IC: " << variable_set_.GetUnrollLoopVariables().GetIc();
  LOG(INFO) << "  KW: " << variable_set_.GetUnrollLoopVariables().GetKw();
  LOG(INFO) << "  KH: " << variable_set_.GetUnrollLoopVariables().GetKh();
  LOG(INFO) << "  OW: " << variable_set_.GetUnrollLoopVariables().GetOw();
  LOG(INFO) << "  OH: " << variable_set_.GetUnrollLoopVariables().GetOh();
  LOG(INFO) << "  OC: " << variable_set_.GetUnrollLoopVariables().GetOc();
  /* #endregion */
}

LoopVariables planner::CnnPlanner::ParseTilingFactor(void)
{
  ifstream tiling_factor_stream(tiling_factor_dump_file_, std::ios::in);

  int stride;
  int input_width;
  int input_height;
  int input_channel;
  int kernel_width;
  int kernel_height;
  int output_width;
  int output_height;
  int output_channel;

  tiling_factor_stream >> stride >> input_width >> input_height >> input_channel
    >> kernel_width >> kernel_height >> output_channel;

  tiling_factor_stream.close();
  
  output_width =
    parameter::ParameterParser::GetOutputLengthCorrespondTo(input_width);
  output_height =
    parameter::ParameterParser::GetOutputLengthCorrespondTo(input_height);

  LoopVariables intra_loop_vars;
  intra_loop_vars.SetStride(stride);
  intra_loop_vars.SetIw(input_width);
  intra_loop_vars.SetIh(input_height);
  intra_loop_vars.SetIc(input_channel);
  intra_loop_vars.SetKw(kernel_width);
  intra_loop_vars.SetKh(kernel_height);
  intra_loop_vars.SetOw(output_width);
  intra_loop_vars.SetOh(output_height);
  intra_loop_vars.SetOc(output_channel);
  /* #region Logging */
  LOG(INFO) << "Parsed stride: " << stride << endl;
  LOG(INFO) << "Parsed input width: " << input_width << endl;
  LOG(INFO) << "Parsed input height: " << input_height << endl;
  LOG(INFO) << "Parsed input channel: " << input_channel << endl;
  LOG(INFO) << "Parsed kernel width: " << kernel_width << endl;
  LOG(INFO) << "Parsed kernel height: " << kernel_height << endl;
  LOG(INFO) << "Parsed output width: " << output_width << endl;
  LOG(INFO) << "Parsed output height: " << output_height << endl;
  LOG(INFO) << "Parsed output channel: " << output_channel << endl;
  /* #endregion */
  return intra_loop_vars;
}

int planner::CnnPlanner::ParseInterLoopSequence(void)
{
  ifstream loop_structure_stream(loop_structure_dump_file_, std::ios::in);
  int inter_loop_seq;
  int intra_loop_seq;

  loop_structure_stream >> inter_loop_seq >> intra_loop_seq;

  loop_structure_stream.close();

  return inter_loop_seq;
}

int planner::CnnPlanner::ParseIntraLoopSequence(void)
{
  ifstream loop_structure_stream(loop_structure_dump_file_, std::ios::in);
  int inter_loop_seq;
  int intra_loop_seq;

  loop_structure_stream >> inter_loop_seq >> intra_loop_seq;

  loop_structure_stream.close();

  return intra_loop_seq;
}

void planner::CnnPlanner::CheckTileSizeWithMemSize(void)
{
  long int input_tile_size =
    variable_set_.GetIntraLoopVariables().GetInputSize() * sizeof(DataType);
  long int weight_tile_size =
    variable_set_.GetIntraLoopVariables().GetWeightSize() * sizeof(DataType);
  long int output_tile_size =
    variable_set_.GetIntraLoopVariables().GetOutputSize() * sizeof(DataType);
  
  if (variable_set_.GetInterLoopVariables().GetInputSize() ==
      variable_set_.GetIntraLoopVariables().GetInputSize()) {
    CHECK(input_tile_size <= architecture_.GetInputMemSize())
      << "Overflow input memory size: " << input_tile_size << " > "
      << architecture_.GetInputMemSize();
  } else {
    CHECK(input_tile_size <= architecture_.GetInputMemSize() / 2)
      << "Overflow input memory size: " << input_tile_size << " > "
      << architecture_.GetInputMemSize() / 2;
  }

  if (variable_set_.GetInterLoopVariables().GetWeightSize() ==
      variable_set_.GetIntraLoopVariables().GetWeightSize()) {
    CHECK(weight_tile_size <= architecture_.GetWeightMemSize())
      << "Overflow weight memory size: " << weight_tile_size << " > "
      << architecture_.GetWeightMemSize();
  } else {
    CHECK(weight_tile_size <= architecture_.GetWeightMemSize() / 2)
      << "Overflow weight memory size: " << weight_tile_size << " > "
      << architecture_.GetWeightMemSize() / 2;
  }

  if (variable_set_.GetInterLoopVariables().GetOutputSize() ==
      variable_set_.GetIntraLoopVariables().GetOutputSize()) {
    CHECK(output_tile_size <= architecture_.GetOutputMemSize())
      << "Overflow output memory size: " << output_tile_size << " > "
      << architecture_.GetOutputMemSize();
  } else {
    CHECK(output_tile_size <= architecture_.GetOutputMemSize() / 2)
      <<  "Overflow output memory size: " << output_tile_size << " > "
      << architecture_.GetOutputMemSize() / 2;
  }
}

void planner::CnnPlanner::GenerateCode(void)
{
  /* #region Logging */
  LOG(INFO) << "Generate simulation code." << endl;
  /* #endregion */
  sim_code_gen_.GenCode(variable_set_,
      inter_loop_structure_, intra_loop_structure_,
      architecture_,
      sim_code_file_, latency_file_, timestamp_file_);
}

void planner::CnnPlanner::DumpTilingInfo(void)
{
  variable_set_.DumpTilingFactor(tiling_factor_dump_file_);
  inter_loop_structure_.Dump(loop_structure_dump_file_);
  intra_loop_structure_.Dump(loop_structure_dump_file_);
}

int planner::CnnPlanner::AnalyzeRealSimulation(void)
{
  /* #region Logging */
  LOG(INFO) << "Analyze real simulation result." << endl;
  /* #endregion */
  report_.SetRealSimulated(true);
  report_.PreAnalyze(variable_set_, inter_loop_structure_,
    intra_loop_structure_, architecture_);
  SetRealLatency();
  report_.Analyze(variable_set_, latency_, architecture_);
  return report_.GetEncodedPartialSumVariables();
}

void planner::CnnPlanner::SetRealLatency(void)
{
  ifstream latency_file_stream(latency_file_);
  CHECK(latency_file_stream.is_open()) << "Latency file: " << latency_file_
    << " is not opened." << endl;
  latency_file_stream >> latency_;
  latency_file_stream.close();
  /* #region Logging */
  LOG(INFO) << "Set real latency." << endl;
  LOG(INFO) << "  Latency: " << latency_ << " ns" << endl;
  /* #endregion */
}

int planner::CnnPlanner::AnalyzeUnrealSimulation(void)
{
  /* #region Logging */
  LOG(INFO) << "Analyze unreal result." << endl;
  /* #endregion */
  report_.SetRealSimulated(false);
  report_.PreAnalyze(variable_set_, inter_loop_structure_,
    intra_loop_structure_, architecture_);
  SetUnrealLatency();
  report_.Analyze(variable_set_, latency_, architecture_);
  return report_.GetEncodedPartialSumVariables();
}

void planner::CnnPlanner::SetUnrealLatency(void)
{
  long int memory_latency = (long int)(
      report_.GetOffChipTotalAccessSize() / architecture_.GetBandwidth());
  long int exe_latency = ceil((double)(report_.GetInterLoopIterations() * 
        report_.GetIntraLoopIterations() * architecture_.GetMacCycles()) /
        architecture_.GetFrequency());
  latency_ = max(memory_latency, exe_latency);

  /* #region Logging */
  LOG(INFO) << "Set unreal latency." << endl;
  LOG(INFO) << "  Memory latency: " << memory_latency << " ns" << endl;
  LOG(INFO) << "  Execution latency: " << exe_latency << " ns" << endl;
  LOG(INFO) << "  Final latency: " << latency_ << " ns" << endl;
  /* #endregion */
}

void planner::CnnPlanner::ReportVerbosly(void) const
{
  /* #region Logging */
  LOG(INFO) << "Report verbosly." << endl;
  /* #endregion */
  PrintLayerParameters();
  PrintTilingParameters();
  PrintUnrollingParameters();
  PrintStationary();
  PrintComputeAnalysisReport();
  PrintDataReuseAnalysisReport();
  PrintOnChipAccessAnalysisReport();
  PrintOffChipAccessAnalysisReport();

  PrintRooflineAnalysisReport();
  PrintEnergyAnalysisReport();
}

void planner::CnnPlanner::ReportSimply(void) const
{
  /* #region Logging */
  LOG(INFO) << "Report simply." << endl;
  /* #endregion */
  PrintRooflineAnalysisReport();
  PrintEnergyAnalysisReport();
}

CsvObject planner::CnnPlanner::GenCsvObject(void) const
{
  return CsvObject(testcase_name_, variable_set_, inter_loop_structure_, 
    report_);
}

void planner::CnnPlanner::PrintConnection(void) const
{
  cout << "------------------------------------------------" << endl
    << "Connecting to CNN Planner back-end..." << endl
    << "------------------------------------------------" << endl;
}

void planner::CnnPlanner::PrintLayerParameters(void) const
{
  cout << endl
    << "Layer Parameters" << endl
    << "------------------------------------------------" << endl;
  variable_set_.GetInterLoopVariables().PrintVariables();
}

void planner::CnnPlanner::PrintTilingParameters(void) const
{
  cout << endl
    << "Tiling Parameters" << endl
    << "------------------------------------------------" << endl;
  variable_set_.GetIntraLoopVariables().PrintVariables();
}

void planner::CnnPlanner::PrintUnrollingParameters(void) const
{
  cout << endl
    << "Unrolling Parameters" << endl
    << "------------------------------------------------" << endl;
  variable_set_.GetUnrollLoopVariables().PrintVariables();
}

void planner::CnnPlanner::PrintStationary(void) const
{
  cout << endl
    << "Stationary" << endl
    << "------------------------------------------------" << endl
    << "[ off-chip :::: on-chip ]" << endl;
  inter_loop_structure_.PrintStationary();
  cout << endl
    << "[ on-chip :::: register file ]" << endl;
  intra_loop_structure_.PrintStationary();
}

void planner::CnnPlanner::PrintComputeAnalysisReport(void) const
{
  cout << endl
    << "Compute Analysis Report" << endl
    << "------------------------------------------------" << endl;
  report_.PrintComputeAnalysisReport();
}

void planner::CnnPlanner::PrintDataReuseAnalysisReport(void) const
{
  cout << endl
    << "Data Reuse analysis Report" << endl
    << "------------------------------------------------" << endl;
  report_.PrintDataReuseAnalysisReport();
}

void planner::CnnPlanner::PrintOnChipAccessAnalysisReport(void) const
{
  cout << endl
    << "On-chip Access Analysis Report" << endl
    << "------------------------------------------------" << endl;
  report_.PrintOnChipAccessAnalysisReport();
}

void planner::CnnPlanner::PrintOffChipAccessAnalysisReport(void) const
{
  cout << endl
    << "Off-chip Access Analysis Report" << endl
    << "------------------------------------------------" << endl;
  report_.PrintOffChipAccessAnalysisReport(); 
}

void planner::CnnPlanner::PrintRooflineAnalysisReport(void) const
{
  cout << endl
    << "Roofline Analysis Report" << endl
    << "------------------------------------------------" << endl;
  report_.PrintRooflineAnalysisReport();
}

void planner::CnnPlanner::PrintEnergyAnalysisReport(void) const
{
  cout << endl
    << "Energy Analysis Report" << endl
    << "------------------------------------------------" << endl;
  report_.PrintEnergyAnalysisReport();
}

const char* planner::CnnPlanner::GetSimulationCodeFile(void) const
{
  return sim_code_file_;
}

const char* planner::CnnPlanner::GetLatencyFile(void) const
{
  return latency_file_;
}

const char* planner::CnnPlanner::GetTimestampFile(void) const
{
  return timestamp_file_;
}

const char* planner::CnnPlanner::GetTetscaseName(void) const
{
  return testcase_name_;
}

const long int planner::CnnPlanner::GetLatency(void) const
{
  return latency_;
}

const LoopScheduler& planner::CnnPlanner::GetLoopScheduler(void) const
{
  return scheduler_;
}

const LoopVariableSet& planner::CnnPlanner::GetLoopVariableSet(void) const
{
  return variable_set_;
}

const LoopStructure& planner::CnnPlanner::GetInterLoopStructure(void) const
{
  return inter_loop_structure_;
}

const LoopStructure& planner::CnnPlanner::GetIntraLoopStructure(void) const
{
  return intra_loop_structure_;
}

const SimulationCodeGenerator& planner::CnnPlanner::GetCodeGenerator(void) const
{
  return sim_code_gen_;
}

const AnalysisReport& planner::CnnPlanner::GetAnalysisReport(void) const
{
  return report_;
}
