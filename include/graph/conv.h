#pragma once

#include "graph/layer.h"
#include "graph/data4d.h"

using graph::layer::Layer;
using graph::data::Data4d;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Convolutional layer.
//! @author   Minsu Kim
//! @date     2020-03-03
////////////////////////////////////////////////////////////////////////////////
class Convolution : public Layer
{
  public:
    Convolution(int ksize, int channel, int stride, int padding)
      : ksize_(ksize), channel_(channel), stride_(stride), padding_(padding) {}

    virtual Data* GetOutput(vector<Data*> inputs) const final
    { return conv(inputs); }

  private:
    const int ksize_;
    const int channel_;
    const int stride_;
    const int padding_;

    Data4d* conv(vector<Data*> inputs) const;
};
} // namespace layer
} // namespace graph