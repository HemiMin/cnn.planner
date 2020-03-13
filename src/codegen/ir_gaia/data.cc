#include "codegen/ir_gaia/data.h"

#include <glog/logging.h>
#include <sstream>

using std::endl;
using std::stringstream;

using codegen::gaia::Data;
using codegen::gaia::DataLayout;

string codegen::gaia::ToString(const DataLayout layout)
{
  switch (layout) {
    case NCHW:  return "NCHW";
    case NHWC:  return "NHWC";
    default: LOG(ERROR) << "Invalid layout type: " << layout;
  }
  return ""; // Here is unreachable.
}
