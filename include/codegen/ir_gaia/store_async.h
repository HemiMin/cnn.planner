#pragma once

#include <iostream>

#include "codegen/ir_gaia/store.h"
#include "codegen/ir_gaia/memory.h"
#include "codegen/ir_gaia/data.h"

using std::ostream;

using codegen::gaia::Store;
using codegen::gaia::Memory;
using codegen::gaia::Data;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Asynchronous store operator class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class AsyncStore : public Store
{
  public:
    AsyncStore(Memory mem, Data operand, int start_addr, int end_addr)
      : Store(mem, operand, start_addr, end_addr) {}
};
ostream& operator<<(ostream& out, const codegen::gaia::AsyncStore& async_store);
} // namespace gaia
} // namespace codegen