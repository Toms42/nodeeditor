#pragma once

#include "Export.hpp"
#include <QPainter>

namespace QtNodes {

class NodeGraphicsObject;
class NodeIndex;
class CompositeGeometry;

/// Class to allow for custom painting
class NODE_EDITOR_PUBLIC NodePainterDelegate {
public:
  virtual ~NodePainterDelegate() = default;

  virtual void paint(QPainter *               painter,
                     const CompositeGeometry &ngo,
                     NodeIndex const &        index) = 0;
};
} // namespace QtNodes
