#pragma once

#include <iostream>

#include "codegen/ir_gaia/load.h"
#include "codegen/ir_gaia/memory.h"
#include "codegen/ir_gaia/data.h"

using std::ostream;

using codegen::gaia::Load;
using codegen::gaia::Memory;
using codegen::gaia::Data;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Asynchronous load operator class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class AsyncLoad : public Load
{
  public:
    AsyncLoad(Memory mem, Data operand, int start_addr, int end_addr)
      : Load(mem, operand, start_addr, end_addr) {}
};
ostream& operator<<(ostream& out, const codegen::gaia::AsyncLoad& async_load);
} // namespace gaia
} // namespace codegen