#pragma once

#include <iostream>

#include "codegen/ir_gaia/op.h"
#include "codegen/ir_gaia/data.h"

using std::ostream;

using codegen::gaia::Operator;
using codegen::gaia::Data;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    ReLU operator class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class Relu : public Operator
{
  public:
    Relu(Data in_operand, Data ot_operand)
      : in_operand_(in_operand),
        ot_operand_(ot_operand) {}

    Data GetInputOperand(void)  const { return in_operand_; }
    Data GetOutputOperand(void) const { return ot_operand_; }

    bool Validate(void) const
    {
      return in_operand_ == ot_operand_;
    }

  private:
    Data in_operand_;
    Data ot_operand_;
};
ostream& operator<<(ostream& out, const codegen::gaia::Relu& relu);
} // namespace gaia
} // namespace codegen
