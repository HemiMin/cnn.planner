#ifndef CNNPLANNER_ANALYSIS_ANALYSIS_REPORT_H_
#define CNNPLANNER_ANALYSIS_ANALYSIS_REPORT_H_

#include <memory>

#include "general/data_type.h"
#include "loop/variable_set.h"
#include "loop/structure.h"
#include "analysis/compute_analyzer.h"
#include "analysis/data_reuse_analyzer.h"
#include "analysis/partial_sum_analyzer.h"
#include "analysis/on_chip_access_analyzer.h"
#include "analysis/off_chip_access_analyzer.h"
#include "analysis/roofline_analyzer.h"
#include "analysis/energy_analyzer.h"
#include "arch/architecture.h"

using std::unique_ptr;

using loop::VariableSet;
using loop::Structure;
using arch::Architecture;

namespace analysis {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Collection of all analysis data.
//! @details  Manage whole analysis data and print report.
//! @author   Minsu Kim
//! @date     2019-07-03
////////////////////////////////////////////////////////////////////////////////
class AnalysisReport {
  public:
    AnalysisReport(void);

    //! @brief                  Inform whether real simulation mode or not.
    //! @param real_simulated   Real simulation mode flag.
    void SetRealSimulated(bool real_simulated);

    //! @brief                        Store analysis data before simulation.
    //! @details                      Analysis data without real latency.
    //! @param variable_set           Set of CNN parameters
    //! @param inter_loop_structure   Inter loop structure 
    //!                               which covers interface between off-chip and on-chip.
    //! @param intra_loop_structure   Intra loop structure
    //!                               which covers interface between on-chip to register file.
    //! @param architecture           Architecture configurations
    void PreAnalyze(const VariableSet& varset,
                    const Structure& off_strt,
                    const Structure& on_strt,
                    const Architecture& arch);
    //! @brief                      Store analysis data after simulation.
    //! @param variable_set         Set of CNN parameters
    //! @param latency              Simulation latency. This can be real when it is --realsim mode
    //!                             and unreal when it is not.
    //! @param architecture         Architecture configurations
    void Analyze( const VariableSet& varset, 
                  long int latency, 
                  const Architecture& arch);

    int GetEncodedPsumVariables(void) const;

    //! @brief    Return register size.
    //! @details  It is figured out from unroll loop iteration factors.
    //!           Register size is calculated in ComputeAnalyzer.
    //! @return   Register size. The unit is Byte.
    int GetRegisterSize(void) const;
    //! @brief    Return the number of operations.
    //! @details  The number of operations is calculated in ComputeAnalyzer.
    //! @return   The number of operations
    long int GetNumOps(void) const;
    //! @brief    Return the number of processing elements.
    //! @return   The number of processing elements
    int GetNumPe(void) const;
    //! @brief    Return the number of off-chip loop iterations.
    //! @details  It depends on the iteration factors of inter loop 
    //!           which covers the interface between off-chip and on-chip.
    //!           Inter loop iterations is calculated in ComputeAnalyzer.
    //! @return   The number of inter loop iterations
    long int GetOffLoopIterations(void) const;
    //! @brief    Return the number of on-chip loop iterations.
    //! @details  It depends on the iteration factors of intra loop
    //!           which covers the interface between on-chip and register file.
    //!           Intra loop iterations is calculated in ComputeAnalyzer.
    //! @return   The number of intra loop iterations
    long int GetOnLoopIterations(void) const;
    //! @brief    Return the optimal execution cycles.
    //! @details  Optimal execution cycles calculated in ComputeAnalyzer.
    //! @return   Optimal execution cycles
    long int GetOptExeCycles(void) const;
    //! @brief    Return the estimated execution cycles.
    //! @details  Estimated execution cycles is based on the total number of iterations.
    //!           Estimated execution cycles is calculated in ComputeAnlayzer.
    //! @return   Estimvated execution cycles
    long int GetEstimatedExeCycles(void) const;
    //! @brief    Return the number of active multiply-accumulator.
    //! @return   The number of active multiply-accumulator
    int GetActiveMacs(void) const;
    //! @brief    Return PE utilization.
    //! @return   PE utilization
    double GetPeUtilization(void) const;

    //! @brief    Return spatial reuse for input data.
    //! @details  Spatial reuse means how many times one data is consumed by processing elements at the same time.
    //!           Spatial input reuse is calculated in DataReuseAnalyzer.
    //! @return   Spatial reuse for input data
    int GetSpatialInputReuse(void) const;
    //! @brief    Return spatial reuse for weight data.
    //! @details  Spatial reuse means how many times one data is consumed by processing elements at the same time.
    //!           Spatial weight reuse is calculated in DataReuseAnalyzer.
    //! @return   Spatial reuse for weight data
    int GetSpatialWeightReuse(void) const;
    //! @brief    Return temporal reuse for input data.
    //! @details  Temporal reuse means how many times one data is reused during iteration before evicted.
    //!           Temporal input reause is calculated in DataReuseAnalyzer.
    //! @return   Temporal reuse for input data
    int GetTemporalInputReuse(void) const;
    //! @brief    Return temporal reuse for weight data.
    //! @details  Temporal reuse means how many times one data is reused during iteration before evicted.
    //!           Temporal input reause is calculated in DataReuseAnalyzer.
    //! @return   Temporal reuse for weight data
    int GetTemporalWeightReuse(void) const;

    //! @brief    Return input buffer size.
    //! @details  Input buffer size is calculated by CNN parameters. The unit is Byte.
    //! @return   Input buffer size    
    long int GetInputBufferSize(void) const;
    //! @brief    Return weight buffer size.
    //! @details  Weight buffer size is calculated by CNN parameters. The unit is Byte.
    //! @return   Weight buffer size    
    long int GetWeightBufferSize(void) const;
    //! @brief    Return output buffer size.
    //! @details  Output buffer size is calculated by CNN parameters. The unit is Byte.
    //! @return   Output buffer size    
    long int GetOutputBufferSize(void) const;
    //! @brief    Return total buffer size.
    //! @details  Total buffer size is summation of whole buffer sizes. The unit is Byte.
    //! @return   Total buffer size    
    long int GetTotalBufferSize(void) const;

    //! @brief    Return on-chip access size for input data during whole layer.
    //! @details  On-chip access size is calculated in OnChipAccessAnalyzer.
    //! @return   On-chip access size for input data
    long int GetOnChipInputLoadSize(void) const;
    //! @brief    Return on-chip access size for weight data during whole layer.
    //! @details  On-chip access size is calculated in OnChipAccessAnalyzer.
    //! @return   On-chip access size for weight data
    long int GetOnChipWeightLoadSize(void) const;
    //! @brief    Return on-chip access size for partial sum data during whole layer.
    //! @details  On-chip access size is calculated in OnChipAccessAnalyzer.
    //! @return   On-chip access size for partial sum data
    long int GetOnChipPartialSumLoadStoreSize(void) const;
    //! @brief    Return on-chip access size for output data during whole layer.
    //! @details  On-chip access size is calculated in OnChipAccessAnalyzer.
    //! @return   On-chip access size for output data
    long int GetOnChipOutputStoreSize(void) const;
    //! @brief    Return total on-chip access size during whole layer.
    //! @details  Total on-chip access size is calculated in OnChipAccessAnalyzer.
    //! @return   Total on-chip access size
    long int GetOnChipTotalAccessSize(void) const;

    //! @brief    Return off-chip access size for input data during whole layer.
    //! @details  Off-chip access size is calculated in OffChipAccessAnalyzer.
    //! @return   Off-chip access size for input data
    long int GetOffChipInputLoadSize(void) const;
    //! @brief    Return off-chip access size for weight data during whole layer.
    //! @details  Off-chip access size is calculated in OffChipAccessAnalyzer.
    //! @return   Off-chip access size for weight data
    long int GetOffChipWeightLoadSize(void) const;
    //! @brief    Return off-chip access size for output data during whole layer.
    //! @details  Off-chip access size is calculated in OffChipAccessAnalyzer.
    //! @return   Off-chip access size for output data
    long int GetOffChipOutputStoreSize(void) const;
    //! @brief    Return total off-chip access size during whole layer.
    //! @details  Total off-chip access size is calculated in OffChipAccessAnalyzer.
    //! @return   Total off-chip access size
    long int GetOffChipTotalAccessSize(void) const;

    //! @brief    Return computation roof in roofline model.
    //! @details  Computation roof is calculated in RooflineAnalyzer.
    //!           The unit is GMACS.
    //! @return   Computation roof  
    double GetComputationRoof(void) const;
    //! @brief    Return optimal arithmetic intensity.
    //! @details  Arithmetic intensity when off-chip memory access is optimal.
    //!           Optimal arithmetic intensity is calculated in RooflineAnalyzer.
    //! @return   Optimal arithmetic intensity
    double GetOptimalArithmeticIntensity(void) const;
    //! @brief    Return actual arithmetic intensity.
    //! @details  Arithmetic intensity is calcuated in RooflineAnalyzer.
    //! @return   Actual arithmetic intensity
    double GetArithmeticIntensity(void) const;
    //! @brief    Return optimal performance.
    //! @details  Performance when optimal arithmetic intensity.
    //!           Optimal performance is calculated in RooflineAnalyzer.
    //!           The unit is GMACS.
    //! @return   Optimal performance
    double GetOptimalPerformance(void) const;
    //! @brief    Return attainable performance.
    //! @details  Performance when actual arithmetic intensity.
    //!           This doesn't consider other factor which affects to performance.
    //!           (e.g. imbalance between multiply and accumulation)
    //!           Attainable performance is calculated in RooflineAnalyzer.
    //!           The unit is GMACS.
    //! @return   Attainable performance
    double GetAttainablePerformance(void) const;
    //! @brief    Return estimated performance.
    //! @details  Compiler estimates performance based on roofline model at compile time.
    //! @return   Estimated performance
    double GetEstimatedPerformance(void) const;
    //! @brief    Return performance.
    //! @details  Real performance based on simulation latency regardless of real mode.
    //!           Performance is calculated in RooflineAnalyzer.
    //!           The unit is GMACS.
    //! @return   Performance
    double GetPerformance(void) const;

    //! @brief    Return off-chip access energy.
    //! @details  Energy consumption for off-chip access during whole layer processing.
    //!           Off-chip access energy is calculated in EnergyAnalyzer.
    //!           The unit is nJ
    //! @return   Off-chip access energy  
    double GetOffChipAccessEnergy(void) const;
    //! @brief    Return on-chip access energy.
    //! @details  Energy consumption for on-chip access during whole layer processing.
    //!           On-chip access energy is calculated in EnergyAnalyzer.
    //!           The unit is nJ
    //! @return   On-chip access energy
    double GetOnChipAccessEnergy(void) const;
    //! @brief    Return execution energy.
    //! @details  Energy consumption for processing element running.
    //!           Execution energy is calculated in EnergyAnalyzer.
    //!           The unit is nJ
    //! @return   Execution energy
    double GetExecutionEnergy(void) const;
    //! @brief    Return total energy consumption during whole layer processing.
    //! @details  Total energy consumption is calculated in EnergyAnalyzer.
    //!           The unit is nJ
    //! @return   Total energy consumptioin
    double GetTotalEnergy(void) const;
    //! @brief    Power consumption during whole layer processing.
    //! @details  Power consumption is calculated in EnergyAnalyzer.
    //!           The unit is nJ
    //! @return   Power consumption
    double GetPower(void) const;

    //! @brief  Print ComputeAnalyzer results.
    ostream& PrintComputeAnalysisReport(ostream& out) const;
    //! @brief  Print DataReuseAnalyzer results.
    ostream& PrintDataReuseAnalysisReport(ostream& out) const;
    //! @brief  Print OnChipAccessAnalyzer results.
    ostream& PrintOnChipAccessAnalysisReport(ostream& out) const;
    //! @brief  Print OffChipAccessAnalzyer results.
    ostream& PrintOffChipAccessAnalysisReport(ostream& out) const;
    //! @brief  Print EnergyAnalyzer results.
    ostream& PrintEnergyAnalysisReport(ostream& out) const;
    //! @brief  Print RooflineAnalyzer results.
    ostream& PrintRooflineAnalysisReport(ostream& out) const;
    //! @brief  Print whole analysis report.
    ostream& PrintAnalysisReport(ostream& out) const;

  private:
    unique_ptr<analysis::ComputeAnalyzer>       comput_anlyzr_;
    unique_ptr<analysis::DataReuseAnalyzer>     data_reuse_anlyzr_;
    unique_ptr<analysis::PartialSumAnalyzer>    psum_anlyzr_;
    unique_ptr<analysis::OnChipAccessAnalyzer>  on_chip_acs_anlyzr_;
    unique_ptr<analysis::OffChipAccessAnalyzer> off_chip_acs_anlyzr_;
    unique_ptr<analysis::RooflineAnalyzer>      roofline_anlyzr_;
    unique_ptr<analysis::EnergyAnalyzer>        energy_anlyzr_;

    int encoded_psum_variables_ = 0;

    int reg_size_                 = NON_VALID;
    int num_pe_                   = NON_VALID;
    long int num_ops_             = NON_VALID;
    long int off_loop_itrs_       = NON_VALID;
    long int on_loop_itrs_        = NON_VALID;
    long int opt_exe_cycles_      = NON_VALID;
    long int estimated_exe_cycles_= NON_VALID;
    int active_macs_              = NON_VALID;
    double pe_util_               = NON_VALID;

    int s_input_reuse_  = NON_VALID;  // spatial input reuse
    int s_weight_reuse_ = NON_VALID;  // spatial weight reuse
    int t_input_reuse_  = NON_VALID;  // temporal input reuse
    int t_weight_reuse_ = NON_VALID;  // temporal weight reuse

    long int i_buf_size_ = NON_VALID; // input buffer size
    long int w_buf_size_ = NON_VALID; // weight buffer size
    long int o_buf_size_ = NON_VALID; // output buffer size
    long int buf_size_   = NON_VALID; // total buffer size

    long int on_chip_input_load_size_     = NON_VALID;
    long int on_chip_weight_load_size_    = NON_VALID;
    long int on_chip_psum_load_store_size_= NON_VALID;
    long int on_chip_output_store_size_   = NON_VALID;
    long int on_chip_total_access_size_   = NON_VALID;

    long int off_chip_input_load_size_  = NON_VALID;
    long int off_chip_weight_load_size_ = NON_VALID;
    long int off_chip_output_store_size_= NON_VALID;
    long int off_chip_total_access_size_= NON_VALID;

    double comput_roof_           = NON_VALID;
    double opt_arith_intns_       = NON_VALID;
    double arith_intns_           = NON_VALID;
    double opt_performance_       = NON_VALID;
    double attainable_performance_= NON_VALID;
    double estimated_performance_ = NON_VALID;
    double performance_           = NON_VALID;

    double off_chip_acs_energy_ = NON_VALID;
    double on_chip_acs_energy_  = NON_VALID;
    double execution_energy_    = NON_VALID;
    double total_energy_        = NON_VALID;
    double power_               = NON_VALID;

    long int DecideInputBufferSize(const VariableSet& varset) const;
    long int DecideWeightBufferSize(const VariableSet& varset) const;

    void CheckValidComputeAnalysisValues(void) const;
    void CheckValidDataReuseAnalysisValues(void) const;
    void CheckValidOnChipAccessAnalysisValues(void) const;
    void CheckValidOffChipAccessAnalysisValues(void) const;
    void CheckValidRooflineAnalysisValues(void) const;
    void CheckValidEnergyAnalysisValues(void) const;
    void CheckValidAnalysisValues(void) const;
};
} // namespace analysis

#endif