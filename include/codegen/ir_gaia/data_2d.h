#pragma once

#include <iostream>

#include "codegen/ir_gaia/data.h"

using std::ostream;

using codegen::gaia::DataLayout;

namespace codegen {
namespace gaia {
////////////////////////////////////////////////////////////////////////////////
//! @brief    2 dimension data class for Gaia intermediate representation.
//! @details  Gaia is name of goddess of Greek mythology.
//! @author   Minsu Kim
//! @date     2020-01-29
////////////////////////////////////////////////////////////////////////////////
class Data2d : public Data
{
  public:
    Data2d(string name, DataLayout layout, int start, int batch, int channel)
      : Data(name, layout, start), batch_(batch), channel_(channel)
    { size_ = (size_t)batch * channel; }

    int GetBatch(void)  const { return batch_;   } 
    int GetChannel(void)const { return channel_; }

    bool operator==(const Data2d& data) const
    { return  data.GetLayout() == layout_ &&
              data.GetBatch() == batch_ &&
              data.GetChannel() == channel_; }

  private:
    int batch_;
    int channel_;
};
ostream& operator<<(ostream& out, codegen::gaia::Data2d& data2d);
} // namespace gaia
} // namespace codegen
