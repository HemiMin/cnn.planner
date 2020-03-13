#pragma once

#include <vector>

#include "graph/vertex.h"
#include "graph/edge.h"

using std::vector;

using graph::Vertex;
using graph::Edge;

namespace graph {
////////////////////////////////////////////////////////////////////////////////
//! @brief    Directed graph.
//! @author   Minsu Kim
//! @date     2020-03-11
////////////////////////////////////////////////////////////////////////////////
class Graph
{
  public:
    ~Graph();

    vector<Vertex*> GetVertices(void) const { return vertices_; }
    vector<Edge*>   GetEdges(void)    const { return edges_;    }

    void AddVertex(Vertex* vertex) { vertices_.push_back(vertex); }
    void AddEdge(Edge* edge)       { edges_.push_back(edge); }

  protected:
    vector<Vertex*> vertices_;
    vector<Edge*> edges_;
};
} // namespace graph