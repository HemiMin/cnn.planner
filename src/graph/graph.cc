#include "graph/graph.h"

using graph::Graph;

Graph::~Graph()
{
  for (Vertex* v : vertices_) delete v;
  for (Edge* e : edges_)      delete e;
}