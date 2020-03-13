#ifndef CNNPLANNER_PLANNER_CNN_PLANNER_H_
#define CNNPLANNER_PLANNER_CNN_PLANNER_H_

#include "parameter/parameter.h"
#include "arch/architecture.h"
#include "general/loop_scheduler.h"
#include "general/loop_variable_set.h"
#include "general/loop_structure.h"
#include "simulation/simulation_code_generator.h"
#include "analysis/analysis_report.h"
#include "statistic/csv_object.h"

#define NON_VALID_VALUE -1

using parameter::Parameter;
using arch::Architecture;
using general::LoopScheduler;
using general::LoopVariableSet;
using general::LoopStructure;
using simulation::SimulationCodeGenerator;
using analysis::AnalysisReport;
using statistic::CsvObject;

namespace planner {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Overall wrapper class wrapping all objects of project
//! @details  Developer can use only this wrapper class to use our service.
//! @author   Minsu Kim
//! @date     2019-07-03
////////////////////////////////////////////////////////////////////////////////
class CnnPlanner
{
  public:
    //! @brief        Constructor using Parameter object and Architecture implementation.
    //! @deatils      Generate local variables of CnnPlanner Class.
    //! @param param  Parameter object which contains CNN parameters parsed by parser.
    CnnPlanner(const Parameter& param);
    //! @brief  Generate simulation source code.
    void GenerateCode(void);
    //! @brief         Dump tiling factors and intra loop structure.
    void DumpTilingInfo(void);
    //! @brief  Analyze on real simulation mode.
    int AnalyzeRealSimulation(void);
    //! @brief  Analyze on unreal simulation mode.
    int AnalyzeUnrealSimulation(void);
    //! @brief  Print report verbosely.
    void ReportVerbosly(void) const;
    //! @brief  Print report simply.
    void ReportSimply(void) const;

    //! @brief              Generate CsvObject using results of reporting.
    //! @return             CsvObject contains results of reporting.
    CsvObject GenCsvObject(void) const;

    //! @brief  Return generated simulation source code path.
    //! @return Generated simulation source code path.
    const char* GetSimulationCodeFile(void) const;
    //! @brief  Return latency recording file path.
    //! @return Latency recording file path.
    const char* GetLatencyFile(void) const;
    //! @brief  Return timestamp file path.
    //! @return Timestamp file path.
    const char* GetTimestampFile(void) const;
    //! @brief    Return test case name.
    //! @details  Actually layer name.
    //! @return   Test case name.
    const char* GetTetscaseName(void) const;

    //! @brief  Return latency value.
    //! @return Latency value.
    const long int GetLatency(void) const;

    //! @brief  Provide reference to access LoopScheduler.
    //! @return LoopScheduler reference.
    const LoopScheduler& GetLoopScheduler(void) const;
    //! @brief  Provide reference to access LoopVariableSet.
    //! @return LoopVariableSet reference.
    const LoopVariableSet& GetLoopVariableSet(void) const;
    //! @brief  Provide reference to access inter loop structure.
    //! @return Inter loop structure.
    const LoopStructure& GetInterLoopStructure(void) const;
    //! @brief  Provide reference to access intra loop structure.
    //! @return Intra loop structure.
    const LoopStructure& GetIntraLoopStructure(void) const;

    //! @brief  Provide reference to access SimulationCodeGenerator.
    //! @return SimulationCodeGenerator reference.
    const SimulationCodeGenerator& GetCodeGenerator(void) const;
    //! @brief  Provide reference to access AnalysisReport.
    //! @return AnalysisReport reference.
    const AnalysisReport& GetAnalysisReport(void) const;

  private:
    LoopScheduler scheduler_;
    LoopVariableSet variable_set_;
    LoopStructure inter_loop_structure_;
    LoopStructure intra_loop_structure_;
    Architecture architecture_;

    //TODO (MinsuKim): Grouping these file paths to structure.
    char sim_code_file_[STR_LEN] = "";
    char latency_file_[STR_LEN] = "";
    char timestamp_file_[STR_LEN] = "";
    char testcase_name_[STR_LEN] = "";
    char tiling_factor_dump_file_[STR_LEN] = "";
    char loop_structure_dump_file_[STR_LEN] = "";

    long int latency_ = NON_VALID_VALUE;

    SimulationCodeGenerator sim_code_gen_;
    AnalysisReport report_;

    void InitLocalVariables(const Parameter& param);
    void InitLocalObjects(const Parameter& param);

    LoopVariables ParseTilingFactor(void);
    int ParseInterLoopSequence(void);
    int ParseIntraLoopSequence(void);

    void CheckTileSizeWithMemSize(void);

    void SetRealLatency(void);
    void SetUnrealLatency(void);

    void PrintConnection(void) const;
    void PrintLayerParameters(void) const;
    void PrintTilingParameters(void) const;
    void PrintUnrollingParameters(void) const;
    void PrintStationary(void) const;
    void PrintComputeAnalysisReport(void) const;
    void PrintDataReuseAnalysisReport(void) const;
    void PrintOnChipAccessAnalysisReport(void) const;
    void PrintOffChipAccessAnalysisReport(void) const;
    void PrintRooflineAnalysisReport(void) const;
    void PrintEnergyAnalysisReport(void) const;
};
} // namespace planner
#endif
