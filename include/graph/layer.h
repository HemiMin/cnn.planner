#pragma once

#include <vector>

#include "graph/data.h"

using std::vector;

using graph::data::Data;

namespace graph {
namespace layer {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Layer of CNN.
//! @author   Minsu Kim
//! @date     2020-03-03
////////////////////////////////////////////////////////////////////////////////
class Layer
{
  public:
    virtual Data* GetOutput(vector<Data*> inputs) const = 0;
    virtual ~Layer() {} 
};
} // namespace layer
} // namespace graph