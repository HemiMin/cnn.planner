#pragma once

#include <stddef.h>

#include "graph/layer.h"
#include "graph/data4d.h"

using std::size_t;

using graph::layer::Layer;
using graph::data::Data4d;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Max pool layer.
//! @author   Minsu Kim
//! @date     2020-03-03
////////////////////////////////////////////////////////////////////////////////
class MaxPool : public Layer
{
  public:
    MaxPool(int ksize, int stride, int padding)
      : ksize_(ksize), stride_(stride), padding_(padding) {}

    Data* GetOutput(vector<Data*> inputs) const final
    { return maxpool(inputs); }

  private:
    const size_t ksize_;
    const size_t stride_;
    const size_t padding_;

    Data4d* maxpool(vector<Data*> inputs) const;
};
} // namespace layer
} // namespace graph