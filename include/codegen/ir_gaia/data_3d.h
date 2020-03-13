#pragma once

#include <iostream>

#include "codegen/ir_gaia/data.h"

using std::ostream;

using codegen::gaia::DataLayout;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    3 dimension data class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class Data3d : public Data
{
  public:
    Data3d( string name, DataLayout layout, int start,
            int channel, int height, int width)
      : Data(name, layout, start), 
        channel_(channel), height_(height), width_(width) 
    { size_ = (size_t)channel*height*width; }

    int GetChannel(void)const { return channel_; }
    int GetHeight(void) const { return height_;  }
    int GetWidth(void)  const { return width_;   }

    bool operator==(const Data3d& data) const
    { return  data.GetLayout() == layout_ &&
              data.GetChannel() == channel_ &&
              data.GetHeight() == height_ &&
              data.GetWidth() == width_; }

  private:
    int channel_;
    int height_;
    int width_;
};
ostream& operator<<(ostream& out, codegen::gaia::Data3d& data3d);
} // namespace gaia
} // namespace codegen
