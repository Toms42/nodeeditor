#pragma once

#include <QtGui/QPainter>

namespace QtNodes {

class NodeState;
class NodeGeometry;
class NodeComposite;
class FlowItemEntry;
class FlowScene;

class NodePainter {
public:
  NodePainter() = default;

public:
  static void paint(QPainter *painter, NodeComposite const &graphicsObject);

  static void drawNodeRect(QPainter *           painter,
                           NodeComposite const &graphicsObject);

  static void drawModelName(QPainter *           painter,
                            NodeComposite const &graphicsObject);

  static void drawEntryLabels(QPainter *           painter,
                              NodeComposite const &graphicsObject);

  static void drawConnectionPoints(QPainter *           painter,
                                   NodeComposite const &graphicsObject);

  static void drawFilledConnectionPoints(QPainter *           painter,
                                         NodeComposite const &graphicsObject);

  static void drawResizeRect(QPainter *           painter,
                             NodeComposite const &graphicsObject);

  static void drawValidationRect(QPainter *           painter,
                                 NodeComposite const &graphicsObject);
};
} // namespace QtNodes
