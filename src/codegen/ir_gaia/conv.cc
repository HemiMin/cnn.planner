#include "codegen/ir_gaia/conv.h"

using codegen::gaia::Convolution;

using std::endl;

ostream& codegen::gaia::operator<<(ostream& out, const Convolution& conv)
{
  out << "CONV    "   << conv.GetOutputOperand().GetName() << "  "
                      << conv.GetInputOperand().GetName() << "  "
                      << conv.GetWeightOperand().GetName() << "  "
                      << conv.GetStride() << "  "
                      << "("  << conv.GetLeftPadding()  << ","
                              << conv.GetRightPadding() << ","
                              << conv.GetUpPadding()    << "," 
                              << conv.GetDownPadding()
                      << ")"  << endl;
  return out;
}