#pragma once

#include "loop/variable_set.h"
#include "loop/structure.h"

using loop::VariableSet;
using loop::Structure;

namespace analysis {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Analyzer for off-chip memory access analysis.
//! @details    Store data access size to off-chip memory.
//! @author     Minsu Kim
//! @date       2019-07-03
////////////////////////////////////////////////////////////////////////////////
class OffChipAccessAnalyzer
{
  public:
    //! @brief                        Return off-chip access size for input data during whole layer.
    //! @param varset                 Set of CNN parameters.
    //! @param off_strt               Off-chip loop structure which provides interface between off-chip and on-chip.
    //! @return                       Off-chip access size for input data
    long int AnalyzeInputLoad(const VariableSet& varset, 
                              const Structure& off_strt) const;
    //! @brief                        Return off-chip access size for weight data during whole layer.
    //! @param varset                 Set of CNN parameters.
    //! @param off_strt               Inter loop structure which provides interface between off-chip and on-chip.
    //! @return                       Off-chip access size for weight data
    long int AnalyzeWeightLoad( const VariableSet& varset, 
                                const Structure& off_strt) const;
    //! @brief                Return off-chip access size for output data during whole layer.
    //! @param varset         Set of CNN parameters.
    //! @return               Off-chip access size for output data
    long int AnalyzeOutputStore(const VariableSet& varset,
                                const Structure& off_strt) const;

  private:
    int AnalyzeInputReload( const VariableSet& varset, 
                            const Structure& off_strt) const;
    int AnalyzeWeightReload(const VariableSet& varset, 
                            const Structure& off_strt) const;
    int AnalyzePartialSumReload(const VariableSet& varset,
                                const Structure& off_strt) const;
};
} // namespace analysis
