#include "codegen/ir_gaia/data_2d.h"

using codegen::gaia::Data2d;

using std::endl;

ostream& codegen::gaia::operator<<(ostream& out, Data2d& data2d)
{
  string ind = "  ";
  out << data2d.GetName()
      << "("  << codegen::gaia::ToString(data2d.GetLayout()) << ","
              << data2d.GetStartIndex() << ","
              << data2d.GetBatch() << ","
              << data2d.GetChannel()
      << ")"  << endl;
  return out;
}
