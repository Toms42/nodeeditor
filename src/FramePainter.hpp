// FramePainter.hpp

#pragma once

class QPainter;

namespace QtNodes {
class NodeComposite;

class FramePainter {
public:
  FramePainter() = default;

  static void paint(QPainter *painter, NodeComposite const &obj);

  static void drawNodeRect(QPainter *           painter,
                           NodeComposite const &graphicsObject);

  static void drawResizeRect(QPainter *           painter,
                             NodeComposite const &graphicsObject);
};
}; // namespace QtNodes
