#pragma once

#include <vector>

namespace codegen {
enum LayerType { CONV=0, RELU, BIAS, BATCH_NORM, MAX_POOL, CONNECTED };
typedef std::vector<LayerType> LayerList;
} // namespace codegen