#pragma once

#include <iostream>

#include "codegen/ir_gaia/op.h"
#include "codegen/ir_gaia/data_1d.h"
#include "codegen/ir_gaia/data_4d.h"

using std::ostream;

using codegen::gaia::Operator;
using codegen::gaia::Data1d;
using codegen::gaia::Data4d;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Bias addition operator class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class Bias : public Operator
{
  public:
    Bias(Data4d in_operand, Data1d bs_operand, Data4d ot_operand)
      : in_operand_(in_operand),
        bs_operand_(bs_operand),
        ot_operand_(ot_operand) {}

    Data4d GetInputOperand(void)  const { return in_operand_; }
    Data1d GetBiasOperand(void)   const { return bs_operand_; }
    Data4d GetOutputOperand(void) const { return ot_operand_; }

    bool Validate(void) const
    {
      return  in_operand_.GetChannel() == bs_operand_.GetChannel() &&
              in_operand_.GetBatch() == ot_operand_.GetBatch() &&
              in_operand_.GetChannel() == ot_operand_.GetChannel() &&
              in_operand_.GetHeight() == ot_operand_.GetHeight() &&
              in_operand_.GetWidth() == ot_operand_.GetWidth();
    }

  private:
    Data4d in_operand_;
    Data1d bs_operand_;
    Data4d ot_operand_;
};
ostream& operator<<(ostream& out, codegen::gaia::Bias& bias);
} // namespace gaia
} // namespace codegen