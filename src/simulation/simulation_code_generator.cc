#include "codegen/simulation_code_generator.h"

#include <glog/logging.h>
#include <iostream>

#include "general/data_type.h"

using codegen::simulation::SimulationCodeGenerator;

using std::endl;

using codegen::simulation::InstructionSlot;
using loop::Location;

void SimulationCodeGenerator::GenCode(const CnnLoop& loop, 
                                      const Architecture& arch)
{
  const VariableSet& varset   = loop.GetVariableSet();
  const Structure&   off_strt = loop.GetOffStructure();
  const Structure&   on_strt  = loop.GetOnStructure();

  ofstream code(code_path_);

  GenPreProcess(code, arch);
  //GenFunctionPrototype(sim_file);
  GenGlobalVariables(code, varset, arch.GetMacCycles());
  GenFunctionDefine(code, off_strt);

  code  << R"(int main(void))" << endl
        << "{" << endl;

  GenSampleDataDeclare(code);
  GenTsStreamOpen(code);

  code  << "\t" << R"(srand(time(NULL));)" << endl
        << endl;

  GenSampleDataInitialization(code);
  GenCalculateBaseline(code);

  GenLocalVariables(code, off_strt);

  GenInterLoop(code, off_strt);
  code
    << "\t\t\t\t\t\t\t" << R"(IsNotFirstIteration = true;)" << endl
    << endl;
  GenIntraLoop(code, on_strt); code << endl;
  GenUnrollLoop(code);
  GenIntraLoopClose(code);
  GenInterLoopClose(code, off_strt);
  GenTsStreamClose(code);

  GenResultCheck(code);
  GenLatencyValueWrite(code);
  GenDelete(code);
  code
    << R"(})" << endl
    << endl;
  // GenFunctionDefine(sim_file, inter_loop_structure);
  code.close();
}

void SimulationCodeGenerator::GenPreProcess(ofstream& code, 
                                            const Architecture& arch)
{
  /* #region Logging */
  LOG(INFO) << "Generate pre-process." << endl;
  /* #endregion */
  code 
    << R"(#include <iostream>)" << endl
    << R"(#include <fstream>)" << endl
    << R"(#include <cstdlib>)" << endl
    << R"(#include <ctime>)" << endl
    << R"(#include <cmath>)" << endl
    << R"(#include <cassert>)" << endl
    << endl;
  if (sizeof(DataType) == 1) {
    /* #region Logging */
    LOG(INFO) << "DataType size: 1 Byte." << endl;
    /* #endregion */
    code
      << R"(typedef unsigned char DataType;)" << endl;
  } else if (sizeof(DataType) == 2) {
    /* #region Logging */
    LOG(INFO) << "DataType size: 2 Bytes." << endl;
    /* #endregion */
    code
      << R"(typedef unsigned short DataType;)" << endl;
  } else if (sizeof(DataType) == 4){
    /* #region Logging */
    LOG(INFO) << "DataType size: 4 Bytes." << endl;
    /* #endregion */
    code
      << R"(typedef unsigned int DataType;)" << endl;
  } else {
    /* #region Logging */
    LOG(FATAL) << "DataType size(" << sizeof(DataType) << ") is not allowed."
      << endl;
    /* #endregion */
  }
  code
    << endl
    << R"(#define MAX_RAND_DATA ( 1 << (sizeof(DataType) * 2) ))"
    << endl
    << R"(#define BANDWIDTH)" << "\t" << arch.GetBandwidth() << endl
    << R"(#define FREQUENCY)" << "\t" << arch.GetFrequency() << endl
    << R"(#define TS_STREAM ")" << ts_file_path_ << R"(")" << endl
    << endl
    << R"(using std::cout;)" << endl
    << R"(using std::endl;)" << endl
    << R"(using std::max;)" << endl
    << R"(using std::min;)" << endl
    << R"(using std::ceil;)" << endl
    << R"(using std::ofstream;)" << endl
    << endl;
}

/*
  void SimulationCodeGenerator::GenFunctionPrototype(ofstream* sim_file)
  {
  sim_file 
  << R"(long int input_load(int Ic, int Oh, int Kh, int Ow, int Kw, long int memory_ts, long int prev_compute_ts);)" << endl
  << R"(long int weight_load(int Oc, int Ic, int Kh, int Kw, long int memory_ts, long int prev_compute_ts);)" << endl
  << R"(long int output_store(int Oc, int Oh, int Ow, long int memory_ts, long int prev_compute_ts);)" << endl
  << R"(long int execute(int exe_cycles, long int compute_ts, long int memory_ts);)" << endl
  << endl;
  }
*/

void SimulationCodeGenerator::GenFunctionDefine(ofstream& code, 
                                                const Structure& off_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate function bodies." << endl;
  /* #endregion */
  GenInputLoadFunctionDefine(  code, off_strt);
  GenWeightLoadFunctionDefine( code, off_strt);
  GenOutputStoreFunctionDefine(code, off_strt);
  GenExecuteFunctionDefine(code);
}

void SimulationCodeGenerator::GenGlobalVariables( ofstream& code, 
                                                  const VariableSet& varset, 
                                                  int mac_cycles)
{
  /* #region Logging */
  LOG(INFO) << "Generate global variables." << endl;
  /* #endregion */
  code
    << R"(const int S = )" << varset.GetStride() << ";" << endl
    << endl
    << R"(const int Iw = )" << varset.GetIw() + 2*varset.GetPw() << ";" << endl
    << R"(const int Ih = )" << varset.GetIh() + 2*varset.GetPh() << ";" << endl
    << R"(const int Ic = )" << varset.GetIc() << ";" << endl
    << R"(const int Kw = )" << varset.GetKw() << ";" << endl
    << R"(const int Kh = )" << varset.GetKh() << ";" << endl
    << R"(const int Ow = )" << varset.GetOw() << ";" << endl
    << R"(const int Oh = )" << varset.GetOh() << ";" << endl
    << R"(const int Oc = )" << varset.GetOc() << ";" << endl
    << endl
    << R"(const int Tiw = )" << varset.GetTiw()+2*varset.GetPw() << ";" << endl
    << R"(const int Tih = )" << varset.GetTih()+2*varset.GetPh() << ";" << endl
    << R"(const int Tic = )" << varset.GetTic() << ";" << endl
    << R"(const int Tkw = )" << varset.GetTkw() << ";" << endl
    << R"(const int Tkh = )" << varset.GetTkh() << ";" << endl
    << R"(const int Tow = )" << varset.GetTow() << ";" << endl
    << R"(const int Toh = )" << varset.GetToh() << ";" << endl
    << R"(const int Toc = )" << varset.GetToc() << ";" << endl
    << endl
    << R"(const int Piw = )" << varset.GetPiw()+2*varset.GetPw() << ";" << endl
    << R"(const int Pih = )" << varset.GetPih()+2*varset.GetPh() << ";" << endl
    << R"(const int Pic = )" << varset.GetPic() << ";" << endl
    << R"(const int Pkw = )" << varset.GetPkw() << ";" << endl
    << R"(const int Pkh = )" << varset.GetPkh() << ";" << endl
    << R"(const int Pow = )" << varset.GetPow() << ";" << endl
    << R"(const int Poh = )" << varset.GetPoh() << ";" << endl
    << R"(const int Poc = )" << varset.GetPoc() << ";" << endl
    << endl
    << R"(const int MacCycles = )" << mac_cycles << ";" << endl
    << endl
    << R"(bool store_flag = false;)" << endl
    << endl;
}

void SimulationCodeGenerator::GenSampleDataDeclare(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate test dataset." << endl;
  /* #endregion */
  code 
    << "\t" << R"(DataType*** input = new DataType**[Ic];)" << endl
    << "\t" << R"(DataType**** weight = new DataType***[Oc];)" << endl
    << "\t" << R"(DataType*** output = new DataType**[Oc];)" << endl
    << "\t" << R"(DataType*** baseline = new DataType**[Oc];)" << endl
    << endl;
}

void SimulationCodeGenerator::GenTsStreamOpen(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate timestamp JSON file: " << ts_file_path_ << endl;
  /* #endregion */
  code
    << "\t" << R"(ofstream ts_stream(TS_STREAM);)" << endl
    << "\t" << R"(ts_stream << "[" << endl;)" << endl;
}

void SimulationCodeGenerator::GenSampleDataInitialization(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate test dataset initializing code." << endl;
  /* #endregion */
  code 
    << "\t" << R"(for ( int ic = 0 ; ic < Ic ; ic++ ) {)" << endl
    << "\t\t" << R"(input[ic] = new DataType*[Ih];)" << endl
    << "\t\t" << R"(for ( int ih = 0 ; ih < Ih ; ih++ ) {)" << endl
    << "\t\t\t" << R"(input[ic][ih] = new DataType[Iw];)" << endl
    << "\t\t\t" << R"(for ( int iw = 0 ; iw < Iw ; iw++ ) {)" << endl
    << "\t\t\t\t" << R"(input[ic][ih][iw] = (DataType)rand() % MAX_RAND_DATA;)" << endl
    << "\t\t\t" << "}" << endl
    << "\t\t" << "}" << endl
    << "\t" << "}" << endl
    << endl
    << "\t" << R"(for ( int oc = 0 ; oc < Oc ; oc++ ) {)" << endl
    << "\t\t" << R"(weight[oc] = new DataType**[Ic];)" << endl
    << "\t\t" << R"(for ( int ic = 0 ; ic < Ic ; ic++ ) {)" << endl
    << "\t\t\t" << R"(weight[oc][ic] = new DataType*[Kh];)" << endl
    << "\t\t\t" << R"(for ( int kh = 0 ; kh < Kh ; kh++ ) {)" << endl
    << "\t\t\t\t" << R"(weight[oc][ic][kh] = new DataType[Kw];)" << endl
    << "\t\t\t\t" << R"(for ( int kw = 0 ; kw < Kw ; kw++ ) {)" << endl
    << "\t\t\t\t\t" << R"(weight[oc][ic][kh][kw] = (DataType)rand() % MAX_RAND_DATA;)" << endl
    << "\t\t\t\t" << "}" << endl
    << "\t\t\t" << "}" << endl
    << "\t\t" << "}" << endl
    << "\t" << "}" << endl
    << endl
    << "\t" << R"(for ( int oc = 0 ; oc < Oc ; oc++ ) {)" << endl
    << "\t\t" << R"(output[oc] = new DataType*[Oh];)" << endl
    << "\t\t" << R"(baseline[oc] = new DataType*[Oh];)" << endl
    << "\t\t" << R"(for ( int oh = 0 ; oh < Oh ; oh++ ) {)" << endl
    << "\t\t\t" << R"(output[oc][oh] = new DataType[Ow];)" << endl
    << "\t\t\t" << R"(baseline[oc][oh] = new DataType[Ow];)" << endl
    << "\t\t\t" << R"(for ( int ow = 0 ; ow < Ow ; ow++ ) {)" << endl
    << "\t\t\t\t" << R"(output[oc][oh][ow] = 0;)" << endl
    << "\t\t\t\t" << R"(baseline[oc][oh][ow] = 0;)" << endl
    << "\t\t\t" << "}" << endl
    << "\t\t" << "}" << endl
    << "\t" << "}" << endl
    << endl;
}

void SimulationCodeGenerator::GenCalculateBaseline(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate baseline calculating code." << endl;
  /* #endregion */
  code 
    << "\t" << R"(for ( int oc = 0 ; oc < Oc ; oc++ ) {)" << endl
    << "\t\t" << R"(for ( int ic = 0 ; ic < Ic ; ic++ ) {)" << endl
    << "\t\t\t" << R"(for ( int oh = 0 ; oh < Oh ; oh++ ) {)" << endl
    << "\t\t\t\t" << R"(for ( int ow = 0 ; ow < Ow ; ow++ ) {)" << endl
    << "\t\t\t\t\t" << R"(for ( int kh = 0 ; kh < Kh ; kh++ ) {)" << endl
    << "\t\t\t\t\t\t" << R"(for ( int kw = 0 ; kw < Kw ; kw++ ) {)" << endl
    << "\t\t\t\t\t\t\t" << R"(baseline[oc][oh][ow] += input[ic][oh*S+kh][ow*S+kw] * weight[oc][ic][kh][kw];)" << endl
    << "\t\t\t\t\t\t" << "}" << endl
    << "\t\t\t\t\t" << "}" << endl
    << "\t\t\t\t" << "}" << endl
    << "\t\t\t" << "}" << endl
    << "\t\t" << "}" << endl
    << "\t" << "}" << endl
    << endl;
}

void SimulationCodeGenerator::GenLocalVariables(ofstream& code, 
                                                const Structure& off_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate local variables." << endl;
  /* #endregion */
  code
    << "\t" << R"(bool IsNotFirstIteration = false;)" << endl
    << endl
    << "\t" << R"(long int memory_ts = 0;)" << endl
    << "\t" << R"(long int prev_compute_ts = 0;)" << endl
    << "\t" << R"(long int compute_ts = 0;)" << endl
    << endl;

  vector<loop::Type> loop_seq = GetLoopSequence(off_strt);

  if (IsOutputStoreSlotUnderOcAndOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store slot is under Oc and Om." << endl;
    /* #endregion */
    code
      << "\t" << R"(int last_oc = 0;)" << endl
      << "\t" << R"(int last_oh = 0;)" << endl
      << "\t" << R"(int last_ow = 0;)" << endl
      << endl;
  } else if (IsOutputStoreSlotUnderOc(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store slot is under Oc." << endl;
    /* #endregion */
    code
      << "\t" << R"(int last_oc = 0;)" << endl
      << endl;
  } else if (IsOutputStoreSlotUnderOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store slot under Om." << endl;
    /* #endregion */
    code
      << "\t" << R"(int last_oh = 0;)" << endl
      << "\t" << R"(int last_ow = 0;)" << endl
      << endl;
  } else if (IsOutputStoreSlotOverOcAndOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store slot over Oc and Om." << endl;
    /* #endregion */
    code
      << endl;
  } else {
    /* #region Logging */
    LOG(FATAL) << "Invalid output store slot location." << endl;
    /* #endregion */
  }

  code
    //<< "\t" << R"(int progress = 0;)" << endl
    << endl;
}

vector<Type> SimulationCodeGenerator::GetLoopSequence(const Structure& strt)
{
  return vector<Type> {
    strt.GetInnerMost(),
    strt.GetSecondInnerMost(),
    strt.GetThirdInnerMost(),
    strt.GetOuterMost()
  };
}

InstructionSlot SimulationCodeGenerator::GetInputLoadSlot(const Structure& strt)
{
  bool pass_input_channel = false;
  bool pass_kernel_map = false;
  bool pass_output_map = false;

  // Don't need to count fully tiled loop.
  if (strt.IsFullyTiled(Type::INPUT_CHANNEL)) {
    /* #region Logging */
    LOG(INFO) << "Input channel is fully tiled." << endl;
    /* #endregion */
    pass_input_channel = true;
  }
  if (strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Kernel map is fully tiled." << endl;
    /* #endregion */
    pass_kernel_map = true;
  }
  if (strt.IsFullyTiled(Type::OUTPUT_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Output map is fully tiled." << endl;
    /* #endregion */
    pass_output_map = true;
  }

  vector<Type> loop_seq = GetLoopSequence(strt);

  for (int ptr = (int)OVER_OUTER_MOST ; ptr >= UNDER_INNER_MOST ; ptr--) {
    if (pass_input_channel && pass_kernel_map && pass_output_map) {
      /* #region Logging */
      LOG(INFO) << "All of loops are fully tiled." << endl;
      /* #endregion */
      return (InstructionSlot)ptr;
    }
    switch (loop_seq[ptr-1]) {
      case Type::INPUT_CHANNEL:
        pass_input_channel = true;
        /* #region Logging */
        LOG(INFO) 
          << "Pass input channel during finding input load slot." << endl;
        /* #endregion */
        break;
      case Type::KERNEL_MAP:
        pass_kernel_map = true;
        /* #region Logging */
        LOG(INFO) 
          << "Pass kerne map during finding input load slot." << endl;
        /* #endregion */
        break;
      case Type::OUTPUT_MAP:
        pass_output_map = true;
        /* #region Logging */
        LOG(INFO) 
          << "Pass output map during finding input load slot." << endl;
        /* #endregion */
        break;
      default:
        break;
    }
  }

  /* #region Logging */
  LOG(FATAL) << "Invalid input load slot pointer." << endl;
  /* #endregion */
}

InstructionSlot SimulationCodeGenerator::GetWeightLoadSlot(
  const Structure& strt)
{
  bool pass_input_channel = false;
  bool pass_kernel_map = false;
  bool pass_output_channel = false;

  // Don't need to count fully tiled loop.
  if (strt.IsFullyTiled(Type::INPUT_CHANNEL)) {
    /* #region Logging */
    LOG(INFO) << "Input channel is fully tiled." << endl;
    /* #endregion */
    pass_input_channel = true;
  }
  if (strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Kernel map is fully tiled." << endl;
    /* #endregion */
    pass_kernel_map = true;
  }
  if (strt.IsFullyTiled(Type::OUTPUT_CHANNEL)) {
    /* #region Logging */
    LOG(INFO) << "Output channel is fully tiled." << endl;
    /* #endregion */
    pass_output_channel = true;
  }

  vector<Type> loop_seq = GetLoopSequence(strt);

  for (int ptr = (int)OVER_OUTER_MOST ; ptr >= UNDER_INNER_MOST ; ptr--) {
    if (pass_input_channel && pass_output_channel && pass_kernel_map) {
      /* #region Logging */
      LOG(INFO) << "All of loops are fully tiled." << endl;
      /* #endregion */
      return (InstructionSlot)ptr;
    }
    switch (loop_seq[ptr-1]) {
      case Type::INPUT_CHANNEL:
        pass_input_channel = true;
        /* #region Logging */
        LOG(INFO)
          << "Pass input channel during finding weight load slot." << endl;
        /* #endregion */
        break;
      case Type::KERNEL_MAP:
        pass_kernel_map = true;
        /* #region Logging */
        LOG(INFO)
          << "Pass kernel map during finding weight load slot." << endl;
        /* #endregion */
        break;
      case Type::OUTPUT_CHANNEL:
        pass_output_channel = true;
        /* #region Logging */
        LOG(INFO)
          << "Pass output channel during finding weight load slot." << endl;
        /* #endregion */
        break;
      default:
        break;
    }
  }

  /* #region Logging */
  LOG(FATAL) << "Invalid weight load slot pointer." << endl;
  /* #endregion */
}

InstructionSlot SimulationCodeGenerator::GetOutputStoreSlot(
  const Structure& strt)
{
  bool pass_input_channel = false;
  bool pass_kernel_map = false;

  // Don't need to count fully tiled loop.
  if (strt.IsFullyTiled(Type::INPUT_CHANNEL)) {
    /* #region Logging */
    LOG(INFO) << "Input channel is fully tiled." << endl;
    /* #endregion */
    pass_input_channel = true;
  }
  if (strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Kernel map is fully tiled." << endl;
    /* #endregion */
    pass_kernel_map = true;
  }

  vector<Type> loop_seq = GetLoopSequence(strt);

  for (int ptr = (int)OVER_OUTER_MOST - loop_seq.size() ; 
      ptr <= OVER_OUTER_MOST ; ptr++) {
    if (pass_input_channel && pass_kernel_map ) {
      /* #region Logging */
      LOG(INFO) << "Both input channel and kernel map are fully tiled." << endl;
      /* #endregion */
      return (InstructionSlot)ptr;
    }
    switch (loop_seq[ptr]) {
      case Type::INPUT_CHANNEL:
        pass_input_channel = true;
        /* #region Logging */
        LOG(INFO)
          << "Pass input channel during finding output store slot." << endl;
        /* #endregion */
        break;
      case Type::KERNEL_MAP:
        pass_kernel_map = true;
        /* #region Logging */
        LOG(INFO)
          << "Pass kernel map during finding output store slot." << endl;
        /* #endregion */
        break;
      default:
        break;
    }
  }

  /* #region Logging */
  LOG(FATAL) << "Invalid output store slot pointer." << endl;
  /* #endregion */
}

void SimulationCodeGenerator::GenInterLoop( ofstream& code, 
                                            const Structure& off_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate inter loop." << endl;
  /* #endregion */
  string indent = "";
  vector<Type> loop_seq = GetLoopSequence(off_strt);

  GenInstructionSlot(code, indent, off_strt, OVER_OUTER_MOST);

  indent += "\t";
  for ( int i = (int)Location::OUTER_MOST ; 
      i >= Location::INNER_MOST ; i-- ) {
    switch (loop_seq[i]) {
      case Type::KERNEL_MAP:
        GenInterKhLoop(code, indent);
        indent += "\t";
        GenInterKwLoop(code, indent);
        GenInstructionSlot(code, indent, off_strt, 
            (InstructionSlot)i);
        break;
      case Type::OUTPUT_MAP:
        GenInterOhLoop(code, indent);
        indent += "\t";
        GenInterOwLoop(code, indent);
        GenInstructionSlot(code, indent, off_strt, 
            (InstructionSlot)i);
        break;
      case Type::INPUT_CHANNEL:
        GenInterIcLoop(code, indent);
        GenInstructionSlot(code, indent, off_strt, 
            (InstructionSlot)i);
        break;
      case Type::OUTPUT_CHANNEL:
        GenInterOcLoop(code, indent);
        GenInstructionSlot(code, indent, off_strt, 
            (InstructionSlot)i);
        break;
      default:
        /* #region Logging */
        LOG(FATAL) << "Invalid loop type." << endl;
        /* #endregion */
    }
    indent += "\t";
  }
}

void SimulationCodeGenerator::GenInstructionSlot( ofstream& code, 
                                                  string indent, 
                                                  const Structure& off_strt, 
                                                  InstructionSlot slot)
{
  if (GetInputLoadSlot(off_strt) == slot) {
    GenInputLoadSlot(code, indent, off_strt);
  }
  if (GetWeightLoadSlot(off_strt) == slot) {
    GenWeightLoadSlot(code, indent, off_strt);
  }
  if (GetOutputStoreSlot(off_strt) == slot) {
    GenStoreFlagSet(code, indent);
  }
}

void SimulationCodeGenerator::GenInputLoadSlot( ofstream& code, 
                                                string indent, 
                                                const Structure& off_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate input load slot." << endl;
  /* #endregion */
  if ( off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::OUTPUT_MAP) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP) ) {
    /* #region Logging */
    LOG(INFO) << "Input load needs no arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = input_load(memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::OUTPUT_MAP) ) {
    /* #region Logging */
    LOG(INFO) << "Input load needs kernel map arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = input_load(kh, kw, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP) ) {
    /* #region Logging */
    LOG(INFO) << "Input load needs output map arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = input_load(oh, ow, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::OUTPUT_MAP) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP) ) {
    /* #region Logging */
    LOG(INFO) << "Input load needs input channel argument." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = input_load(ic, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::INPUT_CHANNEL) ) {
    /* #region Logging */
    LOG(INFO)
      << "Input load needs output map and kernel map arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = input_load(oh, ow, kh, kw, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::OUTPUT_MAP) ) {
    /* #region Logging */
    LOG(INFO) 
      << "Input load needs input channel and kernel map arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = input_load(ic, kh, kw, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::KERNEL_MAP) ) {
    /* #region Logging */
    LOG(INFO) 
      << "Input load needs input channel and output map arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = input_load(ic, oh, ow, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else {
    /* #region Logging */
    LOG(INFO) << "Input load needs all arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = input_load(ic, oh, ow, kh, kw, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  }
}

void SimulationCodeGenerator::GenWeightLoadSlot(ofstream& code, 
                                                string indent, 
                                                const Structure& off_strt)
{
  if ( off_strt.IsFullyTiled(Type::OUTPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP) ) {
    /* #region Logging */
    LOG(INFO) << "Weight load needs no arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = weight_load(memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::OUTPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::INPUT_CHANNEL) ) {
    /* #region Logging */
    LOG(INFO) << "Weight load needs kernel map arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = weight_load(kh, kw, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::OUTPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP) ) {
    /* #region Logging */
    LOG(INFO) << "Weight load needs input channel argument." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = weight_load(ic, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP) ) {
    /* #region Logging */
    LOG(INFO) << "Weight load needs output channel argument." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = weight_load(oc, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::OUTPUT_CHANNEL) ) {
    /* #region Logging */
    LOG(INFO) 
      << "Weight load needs input channel and kernel map arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = weight_load(ic, kh, kw, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::INPUT_CHANNEL) ) {
    /* #region Logging */
    LOG(INFO) 
      << "Weight load needs output channel and kernel map arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = weight_load(oc, kh, kw, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else if ( off_strt.IsFullyTiled(Type::KERNEL_MAP) ) {
    /* #region Logging */
    LOG(INFO) 
      << "Weight load needs output channel and input channel arguments." 
      << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = weight_load(oc, ic, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  } else {
    /* #region Logging */
    LOG(INFO) << "Weight load needs all arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = weight_load(oc, ic, kh, kw, memory_ts, prev_compute_ts, &ts_stream);)" << endl;
  }
}

void SimulationCodeGenerator::GenStoreFlagSet(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate store flag set." << endl;
  /* #endregion */
  code
    << indent << "\t" << R"(if ( IsNotFirstIteration ) {)" << endl
    << indent << "\t\t" << R"(store_flag = true;)" << endl
    << indent << "\t" << R"(})" << endl;
}

bool SimulationCodeGenerator::IsOutputStoreSlotUnderOcAndOm(
  vector<Type> loop_seq)
{
  bool is_under_Oc = false;
  bool is_under_om = false;

  for (int i = (int)Location::OUTER_MOST ; 
      i >= Location::INNER_MOST ; i--) {
    if (loop_seq[i] == Type::OUTPUT_CHANNEL) {
      is_under_Oc = true;
    } else if (loop_seq[i] == Type::OUTPUT_MAP) {
      is_under_om = true;
    } else {
      break;
    }
  }

  return is_under_Oc && is_under_om;
}

bool SimulationCodeGenerator::IsOutputStoreSlotUnderOc(vector<Type> loop_seq)
{
  bool is_under_Oc = false;

  for (int i = (int)Location::OUTER_MOST ; 
      i >= Location::INNER_MOST ; i--) {
    if (loop_seq[i] == Type::OUTPUT_CHANNEL) {
      is_under_Oc = true;
    } else if (loop_seq[i] == Type::OUTPUT_MAP) {
      continue;
    } else {
      break;
    }
  }

  return is_under_Oc;
}

bool SimulationCodeGenerator::IsOutputStoreSlotUnderOm(vector<Type> loop_seq)
{
  bool is_under_om = false;

  for (int i = (int)Location::OUTER_MOST ; 
      i >= Location::INNER_MOST ; i--) {
    if (loop_seq[i] == Type::OUTPUT_CHANNEL) {
      continue;
    } else if (loop_seq[i] == Type::OUTPUT_MAP) {
      is_under_om = true;
    } else {
      break;
    }
  }

  return is_under_om;
}

bool SimulationCodeGenerator::IsOutputStoreSlotOverOcAndOm(
  vector<Type> loop_seq)
{
  bool is_under_Oc = false;
  bool is_under_om = false;

  for (int i = (int)Location::OUTER_MOST ; 
      i >= Location::INNER_MOST ; i--) {
    if (loop_seq[i] == Type::OUTPUT_CHANNEL) {
      is_under_Oc = true;
    } else if (loop_seq[i] == Type::OUTPUT_MAP) {
      is_under_om = true;
    } else {
      break;
    }
  }

  return !is_under_Oc && !is_under_om;
}

void SimulationCodeGenerator::GenInterOcLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate inter output channel loop." << endl;
  /* #endregion */
  code 
    << indent << R"(for ( int oc = 0 ; oc < Oc ; oc += Toc ) {)" << endl;
}

void SimulationCodeGenerator::GenInterIcLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate inter input channel loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int ic = 0 ; ic < Ic ; ic += Tic ) {)" << endl;
}

void SimulationCodeGenerator::GenInterOhLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate inter output height loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int oh = 0 ; oh < Oh ; oh += Toh ) {)" << endl;
}

void SimulationCodeGenerator::GenInterOwLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate inter output width loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int ow = 0 ; ow < Ow ; ow += Tow ) {)" << endl;
}

void SimulationCodeGenerator::GenInterKhLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate inter kernel height loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int kh = 0 ; kh < Kh ; kh += Tkh ) {)" << endl;
}

void SimulationCodeGenerator::GenInterKwLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate inter kernel width loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int kw = 0 ; kw < Kw ; kw += Tkw ) {)" << endl;
}

void SimulationCodeGenerator::GenIntraLoop( ofstream& code, 
                                            const Structure& on_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate intra loop." << endl;
  /* #endregion */
  string indent = "\t\t\t\t\t\t\t";
  vector<Type> loop_seq = GetLoopSequence(on_strt);

  code
    << indent << R"(int exe_cycles = 0;)" << endl;

  for ( int i = (int)Location::OUTER_MOST ; i >= Location::INNER_MOST ; i-- ) {
    switch ( loop_seq[i] ) {
      case Type::KERNEL_MAP:
        GenIntraKhLoop(code, indent);
        indent += "\t";
        GenIntraKwLoop(code, indent);
        break;
      case Type::OUTPUT_MAP:
        GenIntraOhLoop(code, indent);
        indent += "\t";
        GenIntraOwLoop(code, indent);
        break;
      case Type::INPUT_CHANNEL:
        GenIntraIcLoop(code, indent);
        break;
      case Type::OUTPUT_CHANNEL:
        GenIntraOcLoop(code, indent);
        break;
      default:
        /* #region Logging */
        LOG(FATAL) << "Invalid loop type." << endl;
        /* #endregion */
    }
    indent += "\t";
  }
}

void SimulationCodeGenerator::GenIntraOcLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate intra output channel loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int toc = oc ; toc < min(oc+Toc, Oc) ; toc += Poc ) {)" << endl;
}

void SimulationCodeGenerator::GenIntraIcLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate intra input channel loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int tic = ic ; tic < min(ic+Tic, Ic) ; tic += Pic ) {)" << endl;
}

void SimulationCodeGenerator::GenIntraOhLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate intra output height loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int toh = oh ; toh < min(oh+Toh, Oh) ; toh += Poh ) {)" << endl;
}

void SimulationCodeGenerator::GenIntraOwLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate intra output width loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int tow = ow ; tow < min(ow+Tow, Ow) ; tow += Pow ) {)" << endl;
}

void SimulationCodeGenerator::GenIntraKhLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate intra kernel height loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int tkh = kh ; tkh < min(kh+Tkh, Kh) ; tkh += Pkh ) {)" << endl;
}

void SimulationCodeGenerator::GenIntraKwLoop(ofstream& code, string indent)
{
  /* #region Logging */
  LOG(INFO) << "Generate intra kernel width loop." << endl;
  /* #endregion */
  code
    << indent << R"(for ( int tkw = kw ; tkw < min(kw+Tkw, Kw) ; tkw += Pkw ) {)" << endl;
}

void SimulationCodeGenerator::GenIntraLoopClose(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate intra loop closing." << endl;
  /* #endregion */
  code
    << "\t\t\t\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t" << R"(prev_compute_ts = compute_ts;)" << endl
    << "\t\t\t\t\t\t\t" << R"(compute_ts = execute(exe_cycles, compute_ts, memory_ts, &ts_stream);)" << endl
    << endl;
}

void SimulationCodeGenerator::GenUnrollLoop(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate unroll loop." << endl;
  /* #endregion */
  code
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(for ( int poc = toc ; poc < min(toc+Poc, min(oc+Toc, Oc)) ; poc++ ) {)" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(for ( int pic = tic ; pic < min(tic+Pic, min(ic+Tic, Ic)) ; pic++ ) {)" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(for ( int poh = toh ; poh < min(toh+Poh, min(oh+Toh, Oh)) ; poh++ ) {)" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(for ( int pow = tow ; pow < min(tow+Pow, min(ow+Tow, Ow)) ; pow++ ) {)" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(for ( int pkh = tkh ; pkh < min(tkh+Pkh, min(kh+Tkh, Kh)) ; pkh++ ) {)" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(for ( int pkw = tkw ; pkw < min(tkw+Pkw, min(kw+Tkw, Kw)) ; pkw++ ) {)" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(output[poc][poh][pow] += input[pic][poh*S+pkh][pow*S+pkw] * weight[poc][pic][pkh][pkw];)" << endl
    //<< "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(cout << "\r" << ++progress << " / " << Oc * Oh * Ow * Ic * Kh * Kw;)" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(})" << endl
    << "\t\t\t\t\t\t\t\t\t\t\t\t\t" << R"(exe_cycles += MacCycles;)" << endl
    << endl;
}

void SimulationCodeGenerator::GenInterLoopClose(ofstream& code, 
                                                const Structure& off_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate inter loop closing." << endl;
  /* #endregion */
  string indent = "\t\t\t\t\t\t\t";
  vector<Type> loop_seq = GetLoopSequence(off_strt);

  for (int i = (int)Location::INNER_MOST ; 
      i <= Location::OUTER_MOST ; i++) {
    switch (loop_seq[i]) {
      case Type::KERNEL_MAP:
        if (GetOutputStoreSlot(off_strt) == (InstructionSlot)i) {
          GenOutputStoreSlot(code, indent, off_strt);
        }
        indent = indent.substr(1, indent.length()-1);
        code << indent << R"(})" << endl;
        indent = indent.substr(1, indent.length()-1);
        code << indent << R"(})" << endl;
        break;
      case Type::OUTPUT_MAP:
        if (GetOutputStoreSlot(off_strt) == (InstructionSlot)i) {
          GenOutputStoreSlot(code, indent, off_strt);
        }
        indent = indent.substr(1, indent.length()-1);
        code << indent << R"(})" << endl;
        indent = indent.substr(1, indent.length()-1);
        code << indent << R"(})" << endl;
        break;
      case Type::INPUT_CHANNEL:
        if (GetOutputStoreSlot(off_strt) == (InstructionSlot)i) {
          GenOutputStoreSlot(code, indent, off_strt);
        }
        indent = indent.substr(1, indent.length()-1);
        code << indent << R"(})" << endl;
        break;
      case Type::OUTPUT_CHANNEL:
        if (GetOutputStoreSlot(off_strt) == (InstructionSlot)i) {
          GenOutputStoreSlot(code, indent, off_strt);
        }
        indent = indent.substr(1, indent.length()-1);
        code << indent << R"(})" << endl;
        break;
      default:
        /* #region Logging */
        LOG(FATAL) << "Invalid loop type" << endl;
        /* #endregion */
    }
  }

  if (IsOutputStoreSlotUnderOcAndOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs all arguments." << endl;
    /* #endregion */
    code
      << indent << R"(memory_ts = output_store(last_oc, last_oh, last_ow, memory_ts, compute_ts, &ts_stream);)" << endl;
  } else if (IsOutputStoreSlotUnderOc(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs output channel argument." << endl;
    /* #endregion */
    code
      << indent << R"(memory_ts = output_store(last_oc, memory_ts, compute_ts, &ts_stream);)" << endl;
  } else if (IsOutputStoreSlotUnderOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs output map arguments." << endl;
    /* #endregion */
    code
      << indent << R"(memory_ts = output_store(last_oh, last_ow, memory_ts, compute_ts, &ts_stream);)" << endl;
  } else if (IsOutputStoreSlotOverOcAndOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs no argument." << endl;
    /* #endregion */
    code
      << indent << R"(memory_ts = output_store(memory_ts, compute_ts, &ts_stream);)" << endl;
  }
  code
    << endl
    << indent << R"(cout << endl;)" << endl
    << endl;
}

void SimulationCodeGenerator::GenOutputStoreSlot( ofstream& code, 
                                                  string indent, 
                                                  const Structure& off_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate output store slot." << endl;
  /* #endregion */
  vector<Type> loop_seq = GetLoopSequence(off_strt);

  code
    << indent << R"(if ( store_flag ) {)" << endl;
  if (IsOutputStoreSlotUnderOcAndOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs all arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = output_store(last_oc, last_oh, last_ow, memory_ts, prev_compute_ts, &ts_stream);)" << endl
      << indent << "\t" << R"(store_flag = false;)" << endl
      << indent << R"(})" << endl
      << indent << R"(last_oc = oc;)" << endl
      << indent << R"(last_oh = oh;)" << endl
      << indent << R"(last_ow = ow;)" << endl;
  } else if (IsOutputStoreSlotUnderOc(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs output channel argument." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = output_store(last_oc, memory_ts, prev_compute_ts, &ts_stream);)" << endl
      << indent << "\t" << R"(store_flag = false;)" << endl
      << indent << R"(})" << endl
      << indent << R"(last_oc = oc;)" << endl;
  } else if (IsOutputStoreSlotUnderOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "OUtput store needs output map arguments." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = output_store(last_oh, last_ow, memory_ts, prev_compute_ts, &ts_stream);)" << endl
      << indent << "\t" << R"(store_flag = false;)" << endl
      << indent << R"(})" << endl
      << indent << R"(last_oh = oh;)" << endl
      << indent << R"(last_ow = ow;)" << endl;
  } else if (IsOutputStoreSlotOverOcAndOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs no argument." << endl;
    /* #endregion */
    code
      << indent << "\t" << R"(memory_ts = output_store(memory_ts, prev_compute_ts, &ts_stream);)" << endl
      << indent << "\t" << R"(store_flag = false;)" << endl
      << indent << R"(})" << endl;
  } else {
    /* #region Logging */
    LOG(FATAL) << "Invalid output store slot location." << endl;
    /* #endregion */
  }
}

void SimulationCodeGenerator::GenTsStreamClose(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Close timestamp file stream." << endl;
  /* #endregion */
  code
    << "\t" << R"(ts_stream << "{\"type\":\"END\"}" << endl;)" << endl
    << "\t" << R"(ts_stream << "]";)" << endl
    << "\t" << R"(ts_stream.close();)" << endl
    << endl;
}

void SimulationCodeGenerator::GenResultCheck(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate result check." << endl;
  /* #endregion */
  code
    << "\t" << R"(for ( int oc = 0 ; oc < Oc ; oc++ ) {)" << endl
    << "\t\t" << R"(for ( int oh = 0 ; oh < Oh ; oh++ ) {)" << endl
    << "\t\t\t" << R"(for ( int ow = 0 ; ow < Ow ; ow++ ) {)" << endl
    << "\t\t\t\t" << R"(assert(baseline[oc][oh][ow] == output[oc][oh][ow]);)" << endl
    << "\t\t\t" << R"(})" << endl
    << "\t\t" << R"(})" << endl
    << "\t" << R"(})" << endl
    << endl;
}

void SimulationCodeGenerator::GenLatencyValueWrite(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate latency value write." << endl;
  /* #endregion */
  code  << "\t" << R"(ofstream latency_file(")" <<latency_file_path_ << R"(");)"
        << endl << "\t" << R"(latency_file << memory_ts << endl;)"
        << endl << "\t" << R"(latency_file.close();)"
        << endl << endl;
}

void SimulationCodeGenerator::GenInputLoadFunctionDefine(ofstream& code, 
                                                      const Structure& off_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate input load function definition." << endl;
  /* #endregion */
  if (off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::OUTPUT_MAP) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Input load needs no argument." << endl;
    /* #endregion */
    code
      << R"(long int input_load(long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = Tic * Tih * Tiw * sizeof(DataType);)" << endl;
  } else if (off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::OUTPUT_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Input load needs kernel map arguments." << endl;
    /* #endregion */
    code
      << R"(long int input_load(int kh, int kw, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = 0;)" << endl
      << "\t" << R"(if (kw == 0) {)" << endl
      << "\t\t" << R"(load_size = Tic * ((Toh-1)*S + min(Tkh, Kh-kh)) * ((Tow-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl
      << "\t" << R"(} else {)" << endl
      << "\t\t" << R"(load_size = Tic * ((Toh-1)*S + min(Tkh, Kh-kh)) * min(Tkw, Kw-kw) * sizeof(DataType);)" << endl
      << "\t" << R"(})" << endl;
  } else if (off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Input load needs output map arguments." << endl;
    /* #endregion */
    code
      << R"(long int input_load(int oh, int ow, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = 0;)" << endl
      << "\t" << R"(if (ow == 0) {)" << endl
      << "\t\t" << R"(load_size = Tic * ((min(Toh, Oh-oh)-1)*S + Tkh) * ((min(Tow, Ow-ow)-1)*S + Tkw) * sizeof(DataType);)" << endl
      << "\t" << R"(} else {)" << endl
      << "\t\t" << R"(load_size = Tic * (min(Toh, Oh-oh)*S) * ((min(Tow, Ow-ow)-1)*S + Tkw) * sizeof(DataType);)" << endl
      << "\t" << R"(})" << endl;
  } else if (off_strt.IsFullyTiled(Type::OUTPUT_MAP) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Input load needs input channel argument." << endl;
    /* #endregion */
    code
      << R"(long int input_load(int ic, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = min(Tic, Ic-ic) * ((Toh-1)*S + Tkh) * ((Tow-1)*S + Tkw) * sizeof(DataType);)" << endl;
  } else if (off_strt.IsFullyTiled(Type::INPUT_CHANNEL)) {
    /* #region Logging */
    LOG(INFO) << "Input load needs output map arguments." << endl;
    /* #endregion */
    code
      << R"(long int input_load(int oh, int ow, int kh, int kw, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = 0;)" << endl
      << "\t" << R"(if (ow == 0 && kw == 0) {)" << endl
      << "\t\t" << R"(load_size = Tic * ((min(Toh, Oh-oh)-1)*S + min(Tkh, Kh-kh)) * ((min(Tow, Ow-ow)-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl
      << "\t" << R"(} else if (ow == 0) {)" << endl
      << "\t\t" << R"(load_size = Tic * ((min(Toh, Oh-oh)-1)*S + min(Tkh, Kh-kh)) * min(Tkw, Kw-kw) * sizeof(DataType);)" << endl
      << "\t" << R"(} else if (kw == 0) {)" << endl
      << "\t\t" << R"(load_size = Tic * (min(Toh, Oh-oh)*S) * ((min(Tow, Ow-ow)-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl
      << "\t" << R"(} else {)" << endl;
    if (off_strt.GetKernelMap() < 
        off_strt.GetOutputMap()) {
      code
        << "\t\t" << R"(load_size = Tic * min(Tkh, Kh-kh) * ((min(Tow, Ow-ow)-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl;
    } else {
      code
        << "\t\t" << R"(load_size = Tic * (min(Toh, Oh-oh)*S) * ((min(Tow, Ow-ow)-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl;
    }
    code
      << "\t" << R"(})" << endl;
  } else if (off_strt.IsFullyTiled(Type::OUTPUT_MAP)) {
    /* #region Logging */
    LOG(INFO) 
      << "Input load needs input channel and kernel map arguments." << endl;
    /* #endregion */
    code
      << R"(long int input_load(int ic, int kh, int kw, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = 0;)" << endl
      << "\t" << R"(if (kw == 0) {)" << endl
      << "\t\t" << R"(load_size = min(Tic, Ic-ic) * ((Toh-1)*S + min(Tkh, Kh-kh)) * ((Tow-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl
      << "\t" << R"(} else {)" << endl
      << "\t\t" << R"(load_size = min(Tic, Ic-ic) * ((Toh-1)*S + min(Tkh, Kh-kh)) * min(Tkw, Kw-kw) * sizeof(DataType);)" << endl
      << "\t" << R"(})" << endl;
  } else if (off_strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO)
      << "Input load needs input channel and output map arguments." << endl;
    /* #endregion */
    code
      << R"(long int input_load(int ic, int oh, int ow, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = 0;)" << endl
      << "\t" << R"(if (ow == 0) {)" << endl
      << "\t\t" << R"(load_size = min(Tic, Ic-ic) * ((min(Toh, Oh-oh)-1)*S + Tkh) * ((min(Tow, Ow-ow)-1)*S + Tkw) * sizeof(DataType);)" << endl
      << "\t" << R"(} else {)" << endl
      << "\t\t" << R"(load_size = min(Tic, Ic-ic) * (min(Toh, Oh-oh)*S) * ((min(Tow, Ow-ow)-1)*S + Tkw) * sizeof(DataType);)" << endl
      << "\t" << R"(})" << endl;
  } else {
    /* #region Logging */
    LOG(INFO) << "Input load needs all arguments." << endl;
    /* #endregion */
    code
      << R"(long int input_load(int ic, int oh, int ow, int kh, int kw, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = 0;)" << endl
      << "\t" << R"(if (ow == 0 && kw == 0) {)" << endl
      << "\t\t" << R"(load_size = min(Tic, Ic-ic) * ((min(Toh, Oh-oh)-1)*S + min(Tkh, Kh-kh)) * ((min(Tow, Ow-ow)-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl
      << "\t" << R"(} else if (ow == 0) {)" << endl
      << "\t\t" << R"(load_size = min(Tic, Ic-ic) * ((min(Toh, Oh-oh)-1)*S + min(Tkh, Kh-kh)) * min(Tkw, Kw-kw) * sizeof(DataType);)" << endl
      << "\t" << R"(} else if (kw == 0) {)" << endl
      << "\t\t" << R"(load_size = min(Tic, Ic-ic) * (min(Toh, Oh-oh)*S) * ((min(Tow, Ow-ow)-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl
      << "\t" << R"(} else {)" << endl;
    if (off_strt.GetKernelMap() < 
        off_strt.GetOutputMap()) {
      code
        << "\t\t" << R"(load_size = Tic * min(Tkh, Kh-kh) * ((min(Tow, Ow-ow)-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl;
    } else {
      code
        << "\t\t" << R"(load_size = Tic * (min(Toh, Oh-oh)*S) * ((min(Tow, Ow-ow)-1)*S + min(Tkw, Kw-kw)) * sizeof(DataType);)" << endl;
    }
    code
      << "\t" << R"(})" << endl; 
  }

  code
    << "\t" << R"(long int next_ts = max(memory_ts, prev_compute_ts) + ceil((double)load_size / (double)BANDWIDTH);)" << endl
    << "\t" << R"(*ts_stream << "{\"type\":\"MEMORY\", \"datatype\":\"INPUT\", ")" << endl
    << "\t\t" << R"(<< "\"start\":" << max(memory_ts, prev_compute_ts) << ", ")" << endl
    << "\t\t" << R"(<< "\"end\":" << next_ts << ", ")" << endl
    << "\t\t" << R"(<< "\"amount\":" << load_size)" << endl
    << "\t\t" << R"(<< "}," << endl;)" << endl
    << "\t" << R"(return next_ts;)" << endl
    << R"(})" << endl
    << endl;
}

void SimulationCodeGenerator::GenWeightLoadFunctionDefine(ofstream& code, 
                                                      const Structure& off_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate weight load function definition." << endl;
  /* #endregion */
  if (off_strt.IsFullyTiled(Type::OUTPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Weight load needs no argument." << endl;
    /* #endregion */
    code
      << R"(long int weight_load(long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = Toc * Tic * Tkh * Tkw * sizeof(DataType);)" << endl;
  } else if (off_strt.IsFullyTiled(Type::OUTPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::INPUT_CHANNEL)) {
    /* #region Logging */
    LOG(INFO) << "Weight load needs kernel map arguments." << endl;
    /* #endregion */
    code
      << R"(long int weight_load(int kh, int kw, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = Toc * Tic * min(Tkh, Kh-kh) * min(Tkw, Kw-kw) * sizeof(DataType);)" << endl;
  } else if (off_strt.IsFullyTiled(Type::OUTPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Weight load needs input channel argument." << endl;
    /* #endregion */
    code
      << R"(long int weight_load(int ic, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = Toc * min(Tic, Ic-ic) * Tkh * Tkw * sizeof(DataType);)" << endl;
  } else if (off_strt.IsFullyTiled(Type::INPUT_CHANNEL) && 
      off_strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO) << "Weight load needs output channel argument." << endl;
    /* #endregion */
    code
      << R"(long int weight_load(int oc, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = min(Toc, Oc-oc) * Tic * Tkh * Tkw * sizeof(DataType);)" << endl;
  } else if (off_strt.IsFullyTiled(Type::OUTPUT_CHANNEL)) {
    /* #region Logging */
    LOG(INFO) 
      << "Weight load needs input channel and kernel map arguments." << endl;
    /* #endregion */
    code
      << R"(long int weight_load(int ic, int kh, int kw, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = Toc * min(Tic, Ic-ic) * min(Tkh, Kh-kh) * min(Tkw, Kw-kw) * sizeof(DataType);)" << endl;
  } else if (off_strt.IsFullyTiled(Type::INPUT_CHANNEL)) {
    /* #region Logging */
    LOG(INFO)
      << "Weight load needs output channel and kernel map arguments." << endl;
    /* #endregion */
    code
      << R"(long int weight_load(int oc, int kh, int kw, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = min(Toc, Oc-oc) * Tic * min(Tkh, Kh-kh) * min(Tkw, Kw-kw) * sizeof(DataType);)" << endl;
  } else if (off_strt.IsFullyTiled(Type::KERNEL_MAP)) {
    /* #region Logging */
    LOG(INFO)
      << "Weight load needs output channel and input channel arguments." 
      << endl;
    /* #endregion */
    code
      << R"(long int weight_load(int oc, int ic, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = min(Toc, Oc-oc) * min(Tic, Ic-ic) * Tkh * Tkw * sizeof(DataType);)" << endl;
  } else {
    /* #region Logging */
    LOG(INFO) << "Weight load needs all arguments." << endl;
    /* #endregion */
    code
      << R"(long int weight_load(int oc, int ic, int kh, int kw, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int load_size = min(Toc, Oc-oc) * min(Tic, Ic-ic) * min(Tkh, Kh-kh) * min(Tkw, Kw-kw) * sizeof(DataType);)" << endl;
  }

  code
    << "\t" << R"(long int next_ts = max(memory_ts, prev_compute_ts) + ceil((double)load_size / (double)BANDWIDTH);)" << endl
    << "\t" << R"(*ts_stream << "{\"type\":\"MEMORY\", \"datatype\":\"WEIGHT\", ")" << endl
    << "\t\t" << R"(<< "\"start\":" << max(memory_ts, prev_compute_ts) << ", ")" << endl
    << "\t\t" << R"(<< "\"end\":" << next_ts << ", ")" << endl
    << "\t\t" << R"(<< "\"amount\":" << load_size)" << endl
    << "\t\t" << R"(<< "}," << endl;)" << endl
    << "\t" << R"(return next_ts;)" << endl
    << R"(})" << endl
    << endl;
}

void SimulationCodeGenerator::GenOutputStoreFunctionDefine(ofstream& code, 
                                                      const Structure& off_strt)
{
  /* #region Logging */
  LOG(INFO) << "Generate output store function definition." << endl;
  /* #endregion */
  vector<Type> loop_seq = GetLoopSequence(off_strt);

  if (IsOutputStoreSlotUnderOcAndOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs all arguments." << endl;
    /* #endregion */
    code
      << R"(long int output_store(int oc, int oh, int ow, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" <<  endl
      << "\t" << R"(int store_size = min(Toc, Oc-oc) * min(Toh, Oh-oh) * min(Tow, Ow-ow) * sizeof(DataType);)" << endl;
  } else if (IsOutputStoreSlotUnderOc(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs output channel argument." << endl;
    /* #endregion */
    code
      << R"(long int output_store(int oc, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int store_size = min(Toc, Oc-oc) * Oh * Ow * sizeof(DataType);)" << endl;
  } else if (IsOutputStoreSlotUnderOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs output map arguments." << endl;
    /* #endregion */
    code
      << R"(long int output_store(int oh, int ow, long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int store_size = Oc * min(Toh, Oh-oh) * min(Tow, Ow-ow) * sizeof(DataType);)" << endl;
  } else if (IsOutputStoreSlotOverOcAndOm(loop_seq)) {
    /* #region Logging */
    LOG(INFO) << "Output store needs no argument." << endl;
    /* #endregion */
    code
      << R"(long int output_store(long int memory_ts, long int prev_compute_ts, ofstream* ts_stream))" << endl
      << R"({)" << endl
      << "\t" << R"(int store_size = Oc * Oh * Ow * sizeof(DataType);)" << endl;
  } else {
    /* #region Logging */
    LOG(FATAL) << "Invalid output store slot location." << endl;
    /* #endregion */
  }

  code
    << "\t" << R"(long  int next_ts = max(memory_ts, prev_compute_ts) + ceil((double)store_size / (double)BANDWIDTH);)" << endl
    << "\t" << R"(*ts_stream << "{\"type\":\"MEMORY\", \"datatype\":\"OUTPUT\", ")" << endl
    << "\t\t" << R"(<< "\"start\":" << max(memory_ts, prev_compute_ts) << ", ")" << endl
    << "\t\t" << R"(<< "\"end\":" << next_ts << ", ")" << endl
    << "\t\t" << R"(<< "\"amount\":" << store_size)" << endl
    << "\t\t" << R"(<< "}," << endl;)" << endl
    << "\t" << R"(return next_ts;)" << endl
    << R"(})" << endl
    << endl;
}

void SimulationCodeGenerator::GenExecuteFunctionDefine(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate execution function definition." << endl;
  /* #endregion */
  code
    << R"(long int execute(int exe_cycles, long int compute_ts, long int memory_ts, ofstream* ts_stream))" << endl
    << R"({)" << endl
    << "\t" << R"(int num_ops = Tow * Toh * Tic * Tkw * Tkh * Toc;)" << endl
    << "\t" << R"(long int next_ts = max(compute_ts, memory_ts) + ceil((double)exe_cycles / (double)FREQUENCY);)" << endl
    << "\t" << R"(*ts_stream << "{\"type\":\"EXECUTION\", ")" << endl
    << "\t\t" << R"(<< "\"start\":" << max(compute_ts, memory_ts) << ", ")" << endl
    << "\t\t" << R"(<< "\"end\":" << next_ts << ", ")" << endl
    << "\t\t" << R"(<< "\"amount\":" << num_ops)" << endl
    << "\t\t" << R"(<< "}," << endl;)" << endl
    << "\t" << R"(return next_ts;)" << endl
    << R"(})" << endl
    << endl;
}

void SimulationCodeGenerator::GenDelete(ofstream& code)
{
  /* #region Logging */
  LOG(INFO) << "Generate release allocation of dataset." << endl;
  /* #endregion */
  code
    << "\t" << R"(for (int ic = 0 ; ic < Ic ; ic++) {)" << endl
    << "\t\t" << R"(for (int ih = 0 ; ih < Ih ; ih++) {)" << endl
    << "\t\t\t" << R"(delete [] input[ic][ih];)" << endl
    << "\t\t" << R"(})" << endl
    << "\t\t" << R"(delete [] input[ic];)" << endl
    << "\t" << R"(})" << endl
    << "\t" << R"(delete [] input;)" << endl
    << endl
    << "\t" << R"(for (int oc = 0 ; oc < Oc ; oc++) {)" << endl
    << "\t\t" << R"(for (int ic = 0 ; ic < Ic ; ic++) {)" << endl
    << "\t\t\t" << R"(for (int kh = 0 ; kh < Kh ; kh++) {)" << endl
    << "\t\t\t\t" << R"(delete [] weight[oc][ic][kh];)" << endl
    << "\t\t\t" << R"(})" << endl
    << "\t\t\t" << R"(delete [] weight[oc][ic];)" << endl
    << "\t\t" << R"(})" << endl
    << "\t\t" << R"(delete [] weight[oc];)" << endl
    << "\t" << R"(})" << endl
    << "\t" << R"(delete [] weight;)" << endl
    << endl
    << "\t" << R"(for (int oc = 0 ; oc < Oc ; oc++) {)" << endl
    << "\t\t" << R"(for (int oh = 0 ; oh < Oh ; oh++) {)" << endl
    << "\t\t\t" << R"(delete [] output[oc][oh];)" << endl
    << "\t\t\t" << R"(delete [] baseline[oc][oh];)" << endl
    << "\t\t" << R"(})" << endl
    << "\t\t" << R"(delete [] output[oc];)" << endl
    << "\t\t" << R"(delete [] baseline[oc];)" << endl
    << "\t" << R"(})" << endl
    << "\t" << R"(delete [] output;)" << endl
    << "\t" << R"(delete [] baseline;)" << endl;
}