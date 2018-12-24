#pragma once

#include "Export.hpp"
#include <QPainter>

namespace QtNodes {

class NodeGraphicsObject;
class NodeIndex;
class NodeGeometry;

/// Class to allow for custom painting
class NODE_EDITOR_PUBLIC NodePainterDelegate {

public:
  virtual ~NodePainterDelegate() = default;

  virtual void paint(QPainter *painter, NodeGeometry const &ngo,
                     NodeIndex const &index) = 0;
};
} // namespace QtNodes
