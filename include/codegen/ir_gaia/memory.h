#pragma once

#include <iostream>
#include <utility>
#include <string>

#include "codegen/ir_gaia/variable.h"
#include "codegen/ir_gaia/data.h"

using std::ostream;
using std::string;

using codegen::gaia::Variable;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Memory class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @data     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class Memory : public Variable
{
  public:
    Memory(string name, size_t size) : name_(name), size_(size) {}
    Memory(string name, int size) : name_(name), size_((size_t) size) {}

    string GetName(void) const { return name_; }
    size_t GetSize(void) const { return size_; }

  private:
    string name_;
    size_t size_;
};
ostream& operator<<(ostream& out, const codegen::gaia::Memory& mem);
} // namespace gaia
} // namespace codegen
