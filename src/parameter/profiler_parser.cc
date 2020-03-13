#include "parameter/profiler_parser.h"

#include <glog/logging.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "general/utils.h"
#include "general/data_type.h"

using parameter::ProfilerParser;

using std::cout;
using std::endl;

ProfilerParameter* ProfilerParser::BuildParameter(int argc, char** argv)
{
  ProfilerParameter* param = Parsing(argc, argv);
  CheckParameterValid(*param);
  LOG(INFO) << "Success to build profiler parameter.";
  return param;
}

ProfilerParameter* ProfilerParser::Parsing(int argc, char** argv)
{
  ProfilerParameter* param = new ProfilerParameter();
  int opt = 0;
  int opt_index;
  const char* short_opt = "hvg";

  opt = getopt_long(argc, argv, short_opt, p_options, &opt_index);
  while (opt != -1) {
    switch (opt) {
      case 0: // parse long arguments
        ParseLongOptions(opt_index, argv[0], param);
        break;
      case 'h':
        LOG(INFO) << "Parse -h option";
        PrintHelp(argv[0]);
        exit(EXIT_SUCCESS);
      case 'v':
        LOG(INFO) << "Parse -v option";
        param->SetVerbosity(true);
        break;
      case 'g':
        LOG(INFO) << "Parse -g option";
        FLAGS_logtostderr = true;
        break;
      default: break;
    }
    opt = getopt_long(argc, argv, short_opt, p_options, &opt_index);
  }
  return param;
}

void ProfilerParser::ParseLongOptions(int opt_index, char* exe_cmd,
                                      ProfilerParameter* param)
{
  if (strcmp(p_options[opt_index].name, "stride") == 0) {
    kStride = atoi(optarg);
    param->SetStride(kStride);
  } else 
  if (strcmp(p_options[opt_index].name, "iw") == 0) {
    param->SetIw(atoi(optarg));
  } else 
  if (strcmp(p_options[opt_index].name, "ih") == 0) {
    param->SetIh(atoi(optarg));
  } else 
  if (strcmp(p_options[opt_index].name, "ic") == 0) {
    param->SetIc(atoi(optarg));
  } else 
  if (strcmp(p_options[opt_index].name, "pw") == 0) {
    kPadding = atoi(optarg);
    param->SetPw(kPadding);
  } else
  if (strcmp(p_options[opt_index].name, "ph") == 0) {
    param->SetPh(atoi(optarg));
  } else
  if (strcmp(p_options[opt_index].name, "kw") == 0) {
    kFilter_len = atoi(optarg);
    param->SetKw(kFilter_len);
  } else 
  if (strcmp(p_options[opt_index].name, "kh") == 0) {
    kFilter_len = atoi(optarg);
    param->SetKh(kFilter_len);
  } else 
  if (strcmp(p_options[opt_index].name, "oc") == 0) {
    param->SetOc(atoi(optarg));
  } else 
  if (strcmp(p_options[opt_index].name, "mac-cycles") == 0) {
    param->SetMacCycles(atoi(optarg));
  } else 
  if (strcmp(p_options[opt_index].name, "frequency") == 0) {
    param->SetFrequency(atof(optarg));
  } else 
  if (strcmp(p_options[opt_index].name, "bandwidth") == 0) {
    param->SetBandwidth(atof(optarg));
  } else 
  if (strcmp(p_options[opt_index].name, "mac-energy") == 0) {
    param->SetMacEnergy(atof(optarg));
  } else
  if (strcmp(p_options[opt_index].name, "on-chip-32-energy") == 0) {
    param->SetOnChip32Energy(atof(optarg));
  } else 
  if (strcmp(p_options[opt_index].name, "off-chip-32-energy") == 0) {
    param->SetOffChip32Energy(atof(optarg));
  } else 
  if (strcmp(p_options[opt_index].name, "input-mem-size") == 0) {
    param->SetInputMemSize(atof(optarg) * 1024.0);
  } else 
  if (strcmp(p_options[opt_index].name ,"weight-mem-size") == 0) {
    param->SetWeightMemSize(atof(optarg) * 1024.0);
  } else 
  if (strcmp(p_options[opt_index].name, "output-mem-size") == 0) {
    param->SetOutputMemSize(atof(optarg) * 1024.0);
  } else 
  if (strcmp(p_options[opt_index].name, "pe-dim") == 0) {
    param->SetPeDim(Matrix(optarg, strlen(optarg)));
  } else 
  if (strcmp(p_options[opt_index].name, "pe-structure") == 0) {
    param->SetPeStructure(Matrix(optarg, strlen(optarg)));
  } else 
  if (strcmp(p_options[opt_index].name, "latency-path") == 0) {
    param->SetLatencyFile(optarg);
  } else 
  if (strcmp(p_options[opt_index].name, "tiling-dump") == 0) {
    param->SetTilingDumpFile(optarg);
  } else 
  if (strcmp(p_options[opt_index].name, "loop-seq-dump") == 0) {
    param->SetLoopSequenceDumpFile(optarg);
  } else 
  if (strcmp(p_options[opt_index].name, "report-path") == 0) {
    param->SetReportFile(optarg);
  } else 
  if (strcmp(p_options[opt_index].name ,"layer") == 0) {
    param->SetLayerName(optarg);
  } else 
  if (strcmp(p_options[opt_index].name, "help") == 0) {
    PrintHelp(exe_cmd);
    exit(EXIT_SUCCESS);
  }
}

void ProfilerParser::CheckParameterValid(const ProfilerParameter& param) const
{
  CHECK(param.GetStride() > 0) << "Stride is non-valid: " << param.GetStride();
  CHECK(kStride == param.GetStride()) << "kStride is not valid: " << kStride;
  CHECK(param.GetIw() > 0) << "Input width is non-valid: " << param.GetIw();
  CHECK(param.GetIh() > 0) << "Input height is non-valid: " << param.GetIh();
  CHECK(param.GetIc() > 0) << "Input channel is non-valid: " << param.GetIc();
  CHECK(param.GetPw() >= 0) << "Padding width is non-valid: " << param.GetPw();
  CHECK(param.GetPh() >= 0) << "Padding height is non-valid: " << param.GetPh();
  CHECK(param.GetKw() > 0) << "Kernel width is non-valid: " << param.GetKw();
  CHECK(param.GetKh() > 0) << "Kernel height is non-valid: " << param.GetKh();
  CHECK(kFilter_len  == param.GetKw() && kFilter_len == param.GetKh())
    << "KFilter_len is not valid: " << kFilter_len;
  CHECK(param.GetOc() > 0) << "Output channel is non-valid: " << param.GetOc();
  CHECK(param.GetMacCycles() > 0) << "MAC cycles is non-valid: " 
                                  << param.GetMacCycles();
  CHECK(param.GetFrequency() > 0) << "Frequency is non-valid: "
                                  << param.GetFrequency();
  CHECK(param.GetBandwidth() > 0) << "Bandwidth is non-valid: "
                                  << param.GetBandwidth();
  CHECK(param.GetMacEnergy() > 0) << "MAC energy is non-valid: "
                                  << param.GetMacEnergy();
  CHECK(param.GetOnChip32Energy() > 0)  << "On-chip energy is non-valid: "
                                        << param.GetOnChip32Energy();
  CHECK(param.GetOffChip32Energy() > 0) << "Off-chip energy is non-valid: "
                                        << param.GetOffChip32Energy();
  CHECK(param.GetInputMemSize() > 0)  << "Input memory size is non-valid: "
                                      << param.GetInputMemSize();
  CHECK(param.GetWeightMemSize() > 0) << "Weight memory size is non-valid: "
                                      << param.GetWeightMemSize();
  CHECK(param.GetOutputMemSize() > 0) << "Output memory size is non-valid: "
                                      << param.GetOutputMemSize();
  CHECK(strcmp(param.GetLatencyFile(), "") != 0) <<"Latency file is empty.";
  CHECK(strcmp(param.GetTilingDumpFile(), "")!=0)<<"Tiling dump file is empty.";
  CHECK(strcmp(param.GetLoopSequenceDumpFile(), "")!=0)
    << "Loop sequence dump file is empty.";
  CHECK(strcmp(param.GetReportFile(), "") != 0) << "Report file is empty.";
  CHECK(strcmp(param.GetLayerName(), "") != 0) << "Layer name is empty.";
}

void ProfilerParser::PrintHelp(char* exe_cmd) const
{  
  cout    << "e-PlaNNer Profiler."
  << endl << "Usage: " << exe_cmd << " <options>"
  << endl << "where <options> are"
  << endl << "-g                      Debug mode. Show all logs. (not stable)"
  << endl << "-h / --help             Show this help screen."
  << endl << "--stride=<integer>      Stride"
  << endl << "--iw=<integer>          Input width"
  << endl << "--ih=<integer>          Input height"
  << endl << "--ic=<integer>          Input channel"
  << endl << "--pw=<integer>          Padding width"
  << endl << "--ph=<integer>          Padding height"
  << endl << "--kw=<integer>          Kernel width"
  << endl << "--kh=<integer>          Kernel height"
  << endl << "--oc=<integer>          Output channel"
  << endl << "--mac-cycles=<integer>  Parallelization loop hardware cycles"
  << endl << "--frequency=<float>     Hardware frequency"
  << endl << "--bandwidth=<float>     DRAM bandwidth"
  << endl << "--mac-energy=<float>    32-bit MAC computation energy (nJ)"
  << endl << "--on-chip-32-energy=<float>   32-bit data access to on-chip memory (nJ)"
  << endl << "--off-chip-32-energy=<float>  32-bit data access to off-chip memory (nJ)"
  << endl << "--input-mem-size=<float>  Input on-chip memory size (KB)"
  << endl << "--weight-mem-size=<float> Weight on-chip memory size (KB)"
  << endl << "--output-mem-size=<float> Output on-chip memory size (KB)"
  << endl << "--pe-dim=<2D array str>         Physical PE dimension (2D)"
  << endl << "--pe-structure=<2D array str>   PE calculation mapping (2D)"
  << endl << "--latency-path=<path>   Latency file path"
  << endl << "--tiling-dump=<path>    Tiling factor dump file path"
  << endl << "--loop-seq-dump=<path>  Off-chip loop sequence dump file path"
  << endl << "--report-path=<path>    CSV report file path"
  << endl << "--layer=<string>    CNN layer name"
  << endl; 
}