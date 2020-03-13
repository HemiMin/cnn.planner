#ifndef CNNPLANNER_SIMULATION_SIMULATION_CODE_GENERATOR_H_
#define CNNPLANNER_SIMULATION_SIMULATION_CODE_GENERATOR_H_

#include <string.h>
#include <fstream>
#include <vector>
#include <string>

#include "codegen/code_generator.h"
#include "parameter/parameter.h"
#include "loop/cnn_loop.h"
#include "arch/architecture.h"

using std::ofstream;
using std::vector;
using std::string;

using loop::Type;
using loop::CnnLoop;
using arch::Architecture;

namespace codegen {
namespace simulation {
enum InstructionSlot {
  UNDER_INNER_MOST = 0,
  UNDER_SECOND_INNER_MOST, 
  UNDER_THIRD_INNER_MOST,
  UNDER_OUTER_MOST,
  OVER_OUTER_MOST
};

////////////////////////////////////////////////////////////////////////////////
//! @brief      Simulation source code generator.
//! @details    Generate C++ code to run one layer of CNN.
//! @author     Minsu Kim
//! @date       2019-07-03
////////////////////////////////////////////////////////////////////////////////
class SimulationCodeGenerator : public codegen::CodeGenerator
{
  public:
    ~SimulationCodeGenerator() {}

    //! @brief                          Constructor
    //! @param code_path                Simulation source code file path.
    //! @param latnecy_file_path        Latency recording file path.
    //! @param ts_file_path             Timestamp recording file path.  
    SimulationCodeGenerator(const char* code_path,
                            const char* latency_file_path,
                            const char* ts_file_path)
    {
      strncpy(code_path_, code_path, STR_LEN);
      strncpy(latency_file_path_, latency_file_path, STR_LEN);
      strncpy(ts_file_path_, ts_file_path, STR_LEN);
    }

    void GenCode( const CnnLoop& loop, const Architecture& arch);

  private:
    char code_path_[STR_LEN];
    char latency_file_path_[STR_LEN];
    char ts_file_path_[STR_LEN];

    void GenPreProcess( ofstream& code, const Architecture& arch);
    //void GenFunctionPrototype(ofstream& sim_file);
    void GenFunctionDefine(ofstream& code, const Structure& off_strt);
    void GenGlobalVariables(ofstream& code, const VariableSet& varset, 
                            int mac_cycles);
    void GenVariableDeclare(ofstream& code, const VariableSet& varset);
    void GenSampleDataDeclare(ofstream& code);
    void GenTsStreamOpen(ofstream& code);
    void GenSampleDataInitialization(ofstream& code);
    void GenCalculateBaseline(ofstream& code);
    void GenLocalVariables(ofstream& code, const Structure& off_strt);

    vector<loop::Type> GetLoopSequence(const Structure& strt);
    InstructionSlot GetInputLoadSlot(const Structure& strt);
    InstructionSlot GetWeightLoadSlot(const Structure& strt);
    InstructionSlot GetOutputStoreSlot(const Structure& strt);

    void GenInterLoop(ofstream& code, const Structure& off_strt);
    void GenInstructionSlot(ofstream& code, string indent, 
                            const Structure& off_strt, InstructionSlot slot);
        
    void GenInputLoadSlot(ofstream& code, string indent, 
                          const Structure& off_strt);
    void GenWeightLoadSlot(ofstream& code, string indent, 
        const Structure& off_strt);
    void GenStoreFlagSet(ofstream& code, string indent);

    bool IsOutputStoreSlotUnderOcAndOm(vector<loop::Type> loop_seq);
    bool IsOutputStoreSlotUnderOc(vector<loop::Type> loop_seq);
    bool IsOutputStoreSlotUnderOm(vector<loop::Type> loop_seq);
    bool IsOutputStoreSlotOverOcAndOm(vector<loop::Type> loop_seq);

    void GenInterOcLoop(ofstream& code, string indent);
    void GenInterIcLoop(ofstream& code, string indent);
    void GenInterOhLoop(ofstream& code, string indent);
    void GenInterOwLoop(ofstream& code, string indent);
    void GenInterKhLoop(ofstream& code, string indent);
    void GenInterKwLoop(ofstream& code, string indent);

    void GenIntraLoop(ofstream& code, const Structure& on_strt);
    void GenIntraOcLoop(ofstream& code, string indent);
    void GenIntraIcLoop(ofstream& code, string indent);
    void GenIntraOhLoop(ofstream& code, string indent);
    void GenIntraOwLoop(ofstream& code, string indent);
    void GenIntraKhLoop(ofstream& code, string indent);
    void GenIntraKwLoop(ofstream& code, string indent);
    void GenIntraLoopClose(ofstream& code);

    void GenUnrollLoop(ofstream& code);

    void GenInterLoopClose(ofstream& code, const Structure& off_strt);
    void GenOutputStoreSlot(ofstream& code, string indent, 
                            const Structure& off_strt);

    void GenTsStreamClose(ofstream& code);

    void GenResultCheck(ofstream& code);

    void GenLatencyValueWrite(ofstream& code);

    void GenInputLoadFunctionDefine(ofstream& code, const Structure& off_strt);
    void GenWeightLoadFunctionDefine(ofstream& code, const Structure& off_strt);
    void GenOutputStoreFunctionDefine(ofstream& code,const Structure& off_strt);
    void GenExecuteFunctionDefine(ofstream& code);
    void GenDelete(ofstream& code);
};
} // namespace simulation
} // namespace codegen
#endif
