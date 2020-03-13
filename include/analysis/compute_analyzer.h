#pragma once

#include "loop/variable_set.h"

using loop::VariableSet;

namespace analysis {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Analyzer for computation and processing.
//! @details    Store data about operations, iterations and execution.
//! @author     Minsu Kim
//! @date       2019-07-03
////////////////////////////////////////////////////////////////////////////////
class ComputeAnalyzer
{
  public:
    //! @brief                  Return register size.
    //! @details                It is figured out from unroll loop iteration factors.
    //! @param varset           Set of CNN parameters.
    //! @return                 Register size. The unit is Byte.
    int AnalyzeRegisterSize(const VariableSet& varset) const;
    //! @brief                  Return the number of operations.
    //! @param varset           Set of CNN parameters.
    //! @return                 The number of operation
    long int AnalyzeNumOps(const VariableSet& varset) const;
    //! @brief                  Return the number of operations in one tile.
    //! @param varset           Set of CNN parameters.
    //! @return                 The number of operations in one tile.
    long int AnalyzeNumTileOps(const VariableSet& varset) const;
    //! @brief          Return the estimated execution cycles.
    //! @details        Estimated execution cycles is based on the total number of iterations.
    //! @param num_ops  The number of operations.
    //! @param num_pe   The number of processing elements
    //! @return         Estimated execution cycles
    long int AnalyzeOptExeCycles(const long int num_ops,const int num_pe) const;
    //! @brief                  Return the number of off-chip loop iterations.
    //! @details                It depends on the iteration factors of inter loop 
    //!                         which covers the interface between off-chip and on-chip.
    //! @param varset           Set of CNN parameters.
    //! @return                 The number of inter loop iterations
    long int AnalyzeOffLoopIterations(const VariableSet& varset) const;
    //! @brief                  Return the number of on-chip loop iterations.
    //! @details                It depends on the iteration factors of intra loop
    //!                         which covers the interface between on-chip and register file.
    //! @param varset           Set of CNN parameters.
    //! @return                 The number of intra loop iterations
    long int AnalyzeOnLoopIterations(const VariableSet& varset) const;
    //! @brief                  Return the estimated execution cycles.
    //! @details                Estimated execution cycles is based on the total number of iterations.
    //! @param off_loop_itr     Off-chip loop iterations.
    //! @param on_loop_itr      On-chip loop iterations.
    //! @return                 Estimated execution cycles
    long int AnalyzeExeCycles(const long int off_loop_itr, 
                              const long int on_loop_itr) const;
    //! @brief                  Return the number of active multiply-accumulator
    //! @param parl_vars        Parallelization loop variables
    //! @return                 The number of active multiply-accumulator
    int AnalyzeActiveMacs(const Variables& parl_vars, int mac_cycles) const;
};
} // namespace analysis
