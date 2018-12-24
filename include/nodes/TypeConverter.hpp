#pragma once

#include "memory.hpp"
#include <functional>

namespace QtNodes {

class NodeData;

using SharedNodeData = std::shared_ptr<NodeData>;

// a function taking in NodeData and returning NodeData
using TypeConverter = std::function<SharedNodeData(SharedNodeData)>;

} // namespace QtNodes
