#pragma once

#include <iostream>

#include "codegen/ir_gaia/op.h"

using std::ostream;

using codegen::gaia::Operator;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Synchronization operator class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class Sync : public Operator
{
  public:
    Sync(void) {}
    bool Validate(void) const { return true; }
};
ostream& operator<<(ostream& out, const codegen::gaia::Sync& sync);
} // namespace gaia
} // namespace codegen