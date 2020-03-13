#pragma once

#include "loop/variable_set.h"
#include "loop/structure.h"
#include "loop/variables.h"

#define WIDTH_BIT     9
#define HEIGHT_BIT    9
#define CHANNEL_BIT   14

#define WIDTH_BIT_MASK    ~((-1) << (WIDTH_BIT))
#define HEIGHT_BIT_MASK   ~((-1) << (HEIGHT_BIT))
#define CHANNEL_BIT_MASK  ~((-1) << (CHANNEL_BIT))

using loop::VariableSet;
using loop::Structure;
using loop::Type;

namespace analysis {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Analyzer for partial sum.
//! @details    Analyze partial sum dimension
//! @author     Minsu Kim
//! @date       2019-07-05
////////////////////////////////////////////////////////////////////////////////
class PartialSumAnalyzer
{
  public:
    //! @brief                        Get partial sum dimension
    //! @param varset                 Set of CNN parameters.
    //! @param off_strt               Off-chip structure which provides interface between off-chip and on-chip.
    //! @return                       LoopVariables which contain partial sum dimension
    Variables GetPartialSumVariables( const VariableSet& varset,
                                      const Structure& off_strt);
    
    //! @brief                        Get encoded partial sum dimension.
    //! @details                      WIDTH_BIT, HEIGHT_BIT, CHANNEL_BIT sequence from MSB to LSB.
    //! @param varset                 Set of CNN parameters.
    //! @param off_strt               Off-chip loop structure which provides interface between off-chip and on-chip.
    //! @return                       Encoded LoopVariables which contains partial sum dimension
    int GetEncodedPartialSumVariables(const VariableSet& varset,
                                      const Structure& off_strt);

  private:
    int EncodePartialSumVariables(const Variables& psum_var);
    Variables DecodePartialSumVariables(int en_psum_var);
};
} // namespace analysis
