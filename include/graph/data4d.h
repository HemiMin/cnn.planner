#pragma once

#include "graph/data.h"

using graph::data::Data;
using graph::data::DataType;

namespace graph {
namespace data {
////////////////////////////////////////////////////////////////////////////////
//! @brief    A description of intermediate data through DNN.
//! @details  Describe 4-dimensional data.
//! @author   Minsu Kim
//! @date     2020-03-03
////////////////////////////////////////////////////////////////////////////////
class Data4d : public Data
{
  public:
    Data4d( int batch,
            int channel,
            int height, 
            int width, 
            DataType type=DataType::S_FLOAT)
      :     Data(type),
            batch_(batch), 
            channel_(channel), 
            height_(height), 
            width_(width) {}

    int GetBatch(void)   const { return batch_;   }
    int GetChannel(void) const { return channel_; }
    int GetHeight(void)  const { return height_;  }
    int GetWidth(void)   const { return width_;   }

    int GetSize(void) const override
    { return d_size_*batch_*channel_*height_*width_; }

  private:
    const int batch_;
    const int channel_;
    const int height_;
    const int width_;
};
} // namespace data
} // namespace graph