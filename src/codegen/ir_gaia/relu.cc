#include "codegen/ir_gaia/relu.h"

#include <glog/logging.h>

using std::endl;

using codegen::gaia::Relu;

ostream& codegen::gaia::operator<<(ostream& out, const Relu& relu)
{
  out << "RELU    " << relu.GetOutputOperand().GetName() << "  "
                    << relu.GetInputOperand().GetName() << endl;
  return out;
}