#pragma once

#include "graph/data.h"

using graph::data::Data;

namespace graph {
class Vertex;
////////////////////////////////////////////////////////////////////////////////
//! @brief      Edge of Graph data structure.
//! @author     Minsu Kim
//! @date       2020-03-11
////////////////////////////////////////////////////////////////////////////////
class Edge
{
  public:
    Edge(Vertex* from, Vertex* to, Data* data)
      : from_(from), to_(to), data_(data) {}
    ~Edge() { delete data_; }

    void SetFrom(Vertex* from) { from_ = from; }
    void SetTo(Vertex* to)     { to_ = to;     }
    void SetData(Data* data)   { data_ = data; }

    Vertex* GetFrom(void) const { return from_; }
    Vertex* GetTo(void)   const { return to_;   }
    Data* GetData(void)   const { return data_; }

  protected:
    Vertex* from_;
    Vertex* to_;
    Data* data_;
};
} // namespace graph