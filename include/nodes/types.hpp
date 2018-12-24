// types.hpp

#pragma once

#include <QString>
#include <utility>

namespace QtNodes {

struct NodeDataType {
  QString id;
  QString name;
};

enum class ConnectionPolicy { One, Many };

enum class NodeValidationState { Valid, Warning, Error };

// data-type-in, data-type-out
using TypeConverterId = std::pair<NodeDataType, NodeDataType>;

} // namespace QtNodes
