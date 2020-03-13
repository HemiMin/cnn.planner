#pragma once

#include <iostream>

#include "codegen/ir_gaia/op.h"
#include "codegen/ir_gaia/data_4d.h"

using std::ostream;

using codegen::gaia::Operator;
using codegen::gaia::Data4d;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief      Convolution operator class for Gaia intermediate representation.
//! @details    Gaia is name of goddess of Greek mythology.
//! @author     Minsu Kim
//! @date       2020-01-28
////////////////////////////////////////////////////////////////////////////////
class Convolution : public Operator
{
  public:
    Convolution(Data4d in_operand, Data4d wt_operand, Data4d ot_operand, 
                int stride,
                int left_pad, int right_pad, int up_pad, int down_pad) 
      : in_operand_(in_operand), 
        wt_operand_(wt_operand), 
        ot_operand_(ot_operand),
        stride_(stride),
        left_pad_(left_pad),
        right_pad_(right_pad),
        up_pad_(up_pad),
        down_pad_(down_pad) {}

    Data4d GetInputOperand(void)  const { return in_operand_; }
    Data4d GetWeightOperand(void) const { return wt_operand_; }
    Data4d GetOutputOperand(void) const { return ot_operand_; }
    int GetStride(void) const { return stride_; }

    int GetLeftPadding(void)  const { return left_pad_; }
    int GetRightPadding(void) const { return right_pad_;}
    int GetUpPadding(void)    const { return up_pad_;   }
    int GetDownPadding(void)  const { return down_pad_; }

    bool Validate(void) const
    { 
      return stride_ > 0;
    }

  private:
    Data4d in_operand_;
    Data4d wt_operand_;
    Data4d ot_operand_;

    int stride_;

    int left_pad_=0;
    int right_pad_=0;
    int up_pad_=0;
    int down_pad_=0;
};
ostream& operator<<(ostream& out, const codegen::gaia::Convolution& conv);
} // namespace gaia
} // namespace codegen
