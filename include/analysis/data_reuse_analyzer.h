#pragma once

#include "loop/variable_set.h"
#include "loop/structure.h"

using loop::VariableSet;
using loop::Location;

namespace analysis {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Analyzer for data reuse in on-chip memory.
//! @details    Analyze spatial reuse and temporal reuse.
//!             Spatial reuse means how many times one data is consumed by processing element at the same time.
//!             Temporal reuse means how many times one data is reused during iteration before evicted.
//! @author     Minsu Kim
//! @date       2019-07-03
////////////////////////////////////////////////////////////////////////////////
class DataReuseAnalyzer
{
  public:
    //! @brief                  Return spatial reuse for input data.
    //! @param varset           Set of CNN parameters.
    //! @return                 Spatial reuse for input data
    int AnalyzeSpatialInputReuse(const VariableSet& varset) const;
    //! @brief                  Return spatial reuse for weight data.
    //! @param varset           Set of CNN parameters.
    //! @return                 Spatial reuse for weight data
    int AnalyzeSpatialWeightReuse(const VariableSet& varset) const;
    //! @brief                  Return temporal reuse for input data
    //! @param intra_oc         Location of output channel in intra loop.
    //! @param varset           Set of CNN parameters.
    //! @return                 Temporal reuse for input data
    int AnalyzeTemporalInputReuse(const Location intra_oc, 
                                  const VariableSet& varset) const;
    //! @brief                  Return temporal reuse for weight data
    //! @param on_loop_out_map    Location of output map in on-chip loop.
    //! @param varset           Set of CNN parameters.
    //! @return                 Temporal reuse for weight data
    int AnalyzeTemporalWeightReuse( const Location on_loop_out_map, 
                                    const VariableSet& varset) const;
};
} // namespace analysis
