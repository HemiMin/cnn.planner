#pragma once

#include <iostream>

#include "codegen/ir_gaia/op.h"
#include "codegen/ir_gaia/data.h"
#include "codegen/ir_gaia/memory.h"

using std::ostream;

using codegen::gaia::Operator;
using codegen::gaia::Data;
using codegen::gaia::Memory;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Load operator class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-27
////////////////////////////////////////////////////////////////////////////////
class Load : public Operator
{
  public:
    Load(Memory mem, Data operand, int start_addr, int end_addr)
      : mem_(mem), 
        operand_(operand), 
        start_addr_(start_addr), end_addr_(end_addr) {}

    Memory GetMemory(void) const { return mem_; }
    Data GetOperand(void) const { return operand_; }
    int GetStartAddress(void) const { return start_addr_; }
    int GetEndAddress(void) const { return end_addr_; }

    bool Validate(void) const 
    { 
      return  (size_t)end_addr_-start_addr_+1 == operand_.GetSize() &&
              operand_.GetSize() <= mem_.GetSize();
    }

  protected:
    Memory mem_;
    Data operand_;
    int start_addr_;
    int end_addr_;
};
ostream& operator<<(ostream& out, const codegen::gaia::Load& load);
} // namespace gaia
} // namespace codegen
