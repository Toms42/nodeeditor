#pragma once

#include <QtCore/QUuid>
#include <utility>

namespace QtNodes {

enum class PortType { None, In, Out };

using PortIndex = int;

static PortIndex INVALID = -1;

inline PortType oppositePort(PortType port) {
  PortType result = PortType::None;

  switch (port) {
  case PortType::In:
    result = PortType::Out;
    break;

  case PortType::Out:
    result = PortType::In;
    break;

  default:
    break;
  }

  return result;
}
} // namespace QtNodes
