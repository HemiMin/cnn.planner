#pragma once

#include "loop/variable_set.h"

using loop::VariableSet;

namespace analysis {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Analyzer for roofline model.
//! @details    Analyze Arithmetic intensity and performance based on roofline model.
//! @author     Minsu Kim
//! @date       2019-07-03
////////////////////////////////////////////////////////////////////////////////
class RooflineAnalyzer
{
  public:
    //! @brief                  Return actual arithmetic intensity.
    //! @param num_ops          The number of operations.
    //! @param data_transfer    Total number of data transfer from off-chip to on-chip.
    //! @return                 Actual arithmetic intensity
    double AnalyzeArithIntensity( const long int num_ops, 
                                  const long int data_transfer) const;
    //! @brief                  Return optimal arithmetic intensity.
    //! @details                Arithmetic intensity when off-chip memory access is optimal.
    //! @param num_ops          The number of operations.
    //! @param varset           Set of CNN parameters.
    //! @return                 Optimal arithmetic intensity
    double AnalyzeOptArithIntensity(const long int num_ops, 
                                    const VariableSet& varset) const;
    //! @brief                              Return optimal performance.
    //! @details                            Performance when optimal arithmetic intensity.
    //!                                     The unit is GMACS.
    //! @param comput_roof                  Computation roof.
    //! @param opt_arith_intns              Arithmetic intensity when off-chip access is optimal.
    //! @param bandwidth                    Off-chip memory bandwidth.
    //! @return                             Optimal performance
    double AnalyzeOptPerformance( const double comput_roof,
                                  const double opt_arith_intns, 
                                  const double bandwidth) const;
    //! @brief                          Return attainable performance.
    //! @details                        Performance when actual arithmetic intensity.
    //!                                 This doesn't consider other factor which affects to performance.
    //!                                 (e.g. imbalance between multiply and accumulation)
    //!                                 The unit is GMACS.
    //! @param comput_roof              Computation roof.
    //! @param arith_intns              Actual arithmetic intensity.
    //! @param bandwidth                Off-chip memory bandwidth.
    //! @return    Attainable performance
    double AnalyzePerformance(const double comput_roof, 
                              const double arith_intns, 
                              const double bandwidth) const;
};
} // namespace analysis
