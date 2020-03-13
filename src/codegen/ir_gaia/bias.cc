#include "codegen/ir_gaia/bias.h"

using std::endl;

using codegen::gaia::Bias;

ostream& codegen::gaia::operator<<(ostream& out, Bias& bias)
{
  out << "ADDBIAS  "  << bias.GetOutputOperand().GetName() << "  "
                      << bias.GetInputOperand().GetName() << "  "
                      << bias.GetBiasOperand().GetName() << endl;
  return out;
}