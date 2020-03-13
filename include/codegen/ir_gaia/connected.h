#pragma once

#include <iostream>

#include "codegen/ir_gaia/op.h"
#include "codegen/ir_gaia/data_2d.h"

using std::ostream;

using codegen::gaia::Operator;
using codegen::gaia::Data2d;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Fully connected operator class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class Connected : public Operator
{
  public:
    Connected(Data2d in_operand, Data2d wt_operand, Data2d ot_operand)
      : in_operand_(in_operand),
        wt_operand_(wt_operand),
        ot_operand_(ot_operand) {}

    Data2d GetInputOperand(void)  const { return in_operand_; }
    Data2d GetWeightOperand(void) const { return wt_operand_; }
    Data2d GetOutputOperand(void) const { return ot_operand_; }

    bool Validate(void) const 
    { 
      return  in_operand_.GetChannel() == wt_operand_.GetChannel() && 
              ot_operand_.GetChannel() == wt_operand_.GetBatch();
    }

  private:
    Data2d in_operand_;
    Data2d wt_operand_;
    Data2d ot_operand_;
};
ostream& operator<<(ostream& out, const codegen::gaia::Connected& connected);
} // namespace gaia
} // namespace codegen