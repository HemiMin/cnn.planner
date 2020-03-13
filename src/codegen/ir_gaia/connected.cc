#include "codegen/ir_gaia/connected.h"

using codegen::gaia::Connected;

using std::endl;

ostream& codegen::gaia::operator<<(ostream& out, const Connected& connected)
{
  out << "CONNCT  "  << connected.GetOutputOperand().GetName() << "  "
                      << connected.GetInputOperand().GetName() << "  "
                      << connected.GetWeightOperand().GetName() << endl;
  return out;
}