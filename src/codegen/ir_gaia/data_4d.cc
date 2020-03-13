#include "codegen/ir_gaia/data_4d.h"

using codegen::gaia::Data4d;

using std::endl;

ostream& codegen::gaia::operator<<(ostream& out, Data4d& data4d)
{
  string ind = "  ";
  out << data4d.GetName()
      << "("  << codegen::gaia::ToString(data4d.GetLayout()) << ","
              << data4d.GetStartIndex() << ","
              << data4d.GetBatch() << ","
              << data4d.GetChannel() << ","
              << data4d.GetHeight() << ","
              << data4d.GetWidth()
      << ")"  << endl;
  return out;
}
