#ifndef CNNPLANNER_LOOP_CNN_LOOP_H_
#define CNNPLANNER_LOOP_CNN_LOOP_H_

#include <memory>

#include "loop/variable_set.h"
#include "loop/structure.h"

using std::unique_ptr;

using loop::VariableSet;
using loop::Structure;

namespace loop {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Collection of CNN loop components
//! @details    CNN loop contains off-chip loop, on-chip loop, and
//!             parallelization loop variables.
//!             And it also contains inter and intra loop structure.
//! @author     Minsu Kim
//! @date       2019-11-21
////////////////////////////////////////////////////////////////////////////////
class CnnLoop
{
  public:
    //! @brief          Constructor.
    CnnLoop(void);
    //! @brief          Constructor with initial compiler values.
    //! @param param    CompilerParameter.
    CnnLoop(const Parameter& param);
    //! @brief          Copy constructor
    //! @param loop     The instance which is copied.
    CnnLoop(const CnnLoop& loop);
    //! @brief          Set variable set.
    //! @param varset   Variable set instance.
    void SetVariableSet(VariableSet* varset);
    //! @brief          Set off-chip loop structure.
    //! @param off_strt Off-chip loop structure.
    void SetOffStructure(Structure* off_strt);
    //! @brief          Set on-chip loop structure.
    //! @param on_strt  On-chip loop structure.
    void SetOnStructure(Structure* on_strt);

    //! @brief          Get variable set.
    //! @details        Return non-constant reference to update varset easily.
    //! @return         Variable set reference.
    const VariableSet& GetVariableSet(void) const;
    //! @breif          Get off-chip structure.
    //! @details        Return non-constant reference to update off_strt easily.
    //! @return         Off-chip structure.
    const Structure& GetOffStructure(void) const;
    //! @brief          Get on-chip structure.
    //! @details        Return non-constant reference to update on_strt easily.
    //! @return         On-chip structure.
    const Structure& GetOnStructure(void) const;
    //! @brief          Rearrange loop structure.
    //! @details        Move fully tiled loop dimension to innermost.
    void MoveFullyTiledToInnerMost(void);

    //! @brief          Check validation
    void CheckValid(void) const;

  private:
    unique_ptr<VariableSet>  varset_;
    unique_ptr<Structure>    off_strt_;
    unique_ptr<Structure>    on_strt_;
};
} // namespace loop

#endif
