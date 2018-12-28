#pragma once

#include "NodeComposite.hpp"
#include "NodeGeometry.hpp"
#include "types.hpp"
#include <QtCore/QUuid>

namespace QtNodes {

class FlowScene;

/// Class reacts on GUI events, mouse clicks and
/// forwards painting operation.
class NODE_EDITOR_PUBLIC NodeGraphicsObject final : public NodeComposite {
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

  /**\return proxyWidget of current node
   */
  QGraphicsProxyWidget *      proxyWidget();
  const QGraphicsProxyWidget *proxyWidget() const;

  CompositeGeometry &      geometry() override;
  const CompositeGeometry &geometry() const override;

protected:
  void paint(QPainter *                      painter,
             const QStyleOptionGraphicsItem *option,
             QWidget *                       widget = nullptr) override;

  void embedQWidget();

protected:
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &   value) override;

  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
  QGraphicsProxyWidget *proxyWidget_;
  NodeGeometry          geometry_;
};
} // namespace QtNodes
