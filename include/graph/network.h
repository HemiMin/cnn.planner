#pragma once

#include "graph/graph.h"
#include "graph/vertex.h"

using graph::Graph;
using graph::Vertex;

namespace graph {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Describe connections of DNN.
//! @author   Minsu Kim
//! @date     2020-03-11
////////////////////////////////////////////////////////////////////////////////
class Network
{
  public:
    ~Network() { delete network_; }

    void SetNetwork(Graph* network) { network_ = network; }
    void SetHeader(Vertex* header)  { header_ = header;   }
    void SetTrailer(Vertex* trailer){ trailer_ = trailer; }

    Graph* GetNetwork(void) const { return network_; }
    Vertex* GetHeader(void) const { return header_;  }
    Vertex* GetTrailer(void) const{ return trailer_; }

  private:
    Graph* network_;
    Vertex* header_ = nullptr;
    Vertex* trailer_ = nullptr;
};
} // namespace graph