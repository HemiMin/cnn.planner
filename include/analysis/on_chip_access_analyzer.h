#pragma once

#include "loop/variable_set.h"

using loop::VariableSet;

namespace analysis {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Analyzer for on-chip memory access analysis.
//! @details    Store data access size to on-chip memory.
//! @author     Minsu Kim
//! @date       2019-07-03
////////////////////////////////////////////////////////////////////////////////
class OnChipAccessAnalyzer
{
  public:
    //! @brief                          Return on-chip access size for weight data during whole layer.
    //! @param num_ops                  The number of operations.
    //! @param s_weight_reuse           Spatial reuse for weight data.
    //! @param t_weight_reuse           Temporal reuse for weight data.
    //! @return                         On-chip access size for weight data
    long int AnalyzeWeightLoad( const long int num_ops, 
                                const int s_weight_reuse, 
                                const int t_weight_reuse) const;
    //! @brief                          Return on-chip access size for input data during whole layer.
    //! @param num_ops                  The number of operations.
    //! @param s_weight_reuse           Spatial reuse for weight data.
    //! @param t_weight_reuse           Temporal reuse for weight data.
    //! @return                         On-chip access size for input data
    long int AnalyzeInputLoad(const long int num_ops, 
                              const int s_input_reuse, 
                              const int t_input_reuse) const;
    //! @brief                  Return on-chip access size for partial sum data during whole layer.
    //! @param exe_cycles       Execution cycles
    //! @param varset           Set of CNN parameters
    //! @return                 On-chip access size for partial sum data
    long int AnalyzePartialSumAccess( const long int exe_cycles, 
                                      const VariableSet& varset) const;
    //! @brief                  Return on-chip access size for output data during whole layer.
    //! @param varset           Set of CNN parameters
    //! @return                 On-chip access size for output data
    long int AnalyzeOutputStore(const VariableSet& varset) const;
    //! @brief                      Return total on-chip access size during whole layer.
    //! @param input_load           On-chip access size for input data.
    //! @param weight_load          On-chip access size for weight data.
    //! @param output_store         On-chip access size for output data.
    //! @param p_sum_access         On-chip access size for partial sum.
    //! @return                     Total on-chip access size
    long int AnalyzeTotalAccess(const long int input_load, 
                                const long int weight_load, 
                                const long int output_store, 
                                const long int p_sum_access) const;
};
} // namespace analysis
