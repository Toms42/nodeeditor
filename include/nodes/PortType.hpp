#pragma once

#include "types.hpp"
#include <QtCore/QUuid>
#include <utility>

namespace QtNodes {

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
