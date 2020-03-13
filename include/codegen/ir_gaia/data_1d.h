#pragma once

#include <iostream>

#include "codegen/ir_gaia/data.h"

using std::ostream;

using codegen::gaia::DataLayout;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    1 dimension data class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class Data1d : public Data
{
  public:
    Data1d(string name, DataLayout layout, int start, int channel)
      : Data(name, layout, start), channel_(channel) {size_ = (size_t)channel;}
    
    int GetChannel(void) const { return channel_; }

    bool operator==(const Data1d& data) const 
    { return data.GetLayout() == layout_ && data.GetChannel() == channel_; }

  private:
    int channel_;
};
ostream& operator<<(ostream& out, codegen::gaia::Data1d& data1d);
} // namespace gaia
} // namespace codegen
