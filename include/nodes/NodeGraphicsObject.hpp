#pragma once

#include "NodeComposite.hpp"
#include "types.hpp"
#include <QtCore/QUuid>

class QGraphicsProxyWidget;

namespace QtNodes {

class FlowScene;

/// Class reacts on GUI events, mouse clicks and
/// forwards painting operation.
class NODE_EDITOR_PUBLIC NodeGraphicsObject : public NodeComposite {
  Q_OBJECT

public:
  NodeGraphicsObject(FlowScene &scene, NodeIndex const &index);

  ~NodeGraphicsObject() override;

  /// Visits all attached connections and corrects
  /// their corresponding end points.
  virtual void moveConnections() const;

  virtual void reactToPossibleConnection(PortType,
                                         NodeDataType,
                                         QPointF const &scenePoint) override;

  virtual void resetReactionToConnection() override;

  int type() const override;

protected:
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &   value) override;

  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};
} // namespace QtNodes
