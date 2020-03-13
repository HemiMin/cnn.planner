#pragma once

#include "graph/data.h"

using graph::data::Data;
using graph::data::DataType;

namespace graph {
namespace data {
////////////////////////////////////////////////////////////////////////////////
//! @brief    A description of intermediate data through DNN.
//! @details  Describe 2-dimensional data.
//! @author   Minsu Kim
//! @date     2020-03-04 
////////////////////////////////////////////////////////////////////////////////
class Data2d : public Data
{
  public:
    Data2d(int batch, int channel, DataType type=DataType::S_FLOAT)
      : Data(type), batch_(batch), channel_(channel) {}

    int GetBatch(void)   const { return batch_; }
    int GetChannel(void) const { return channel_; }

    int GetSize(void) const override { return d_size_ * batch_ * channel_; }

  private:
    const int batch_;
    const int channel_;
};
} // namespace data
} // namespace graph