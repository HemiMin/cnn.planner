#include "codegen/ir_gaia/data_1d.h"

using codegen::gaia::Data1d;

using std::endl;

ostream& codegen::gaia::operator<<(ostream& out, Data1d& data1d)
{
  string ind = "  ";
  out << data1d.GetName()
      << "("  << codegen::gaia::ToString(data1d.GetLayout()) << ","
              << data1d.GetStartIndex() << ","
              << data1d.GetChannel()
      << ")"  << endl;
  return out;
}
