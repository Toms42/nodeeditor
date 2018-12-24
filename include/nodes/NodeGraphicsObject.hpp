#ifndef NODES_NODEGRAPHICSOBJECT_HPP
#define NODES_NODEGRAPHICSOBJECT_HPP

#pragma once

#include "NodeGeometry.hpp"
#include "NodeState.hpp"
#include "types.hpp"
#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

class QGraphicsProxyWidget;

namespace QtNodes {

class FlowScene;

/// Class reacts on GUI events, mouse clicks and
/// forwards painting operation.
class NODE_EDITOR_PUBLIC NodeGraphicsObject : public QGraphicsObject {
  Q_OBJECT

  /**\brief NodeGeometry set boundingRect for the class, so, every time,
   * when values, which neded for change boundingRect, changes - we have to call
   * prepareGeometryChange. So we can call this function in NodeGeometry -
   * this is simple and we can forget about do it by hand. And it fix problems
   */
  friend NodeGeometry;

public:
  NodeGraphicsObject(FlowScene &scene, NodeIndex const &index);

  ~NodeGraphicsObject() override;

  NodeIndex index() const;

  FlowScene &flowScene();

  FlowScene const &flowScene() const;

  NodeGeometry &geometry();

  NodeGeometry const &geometry() const;

  NodeState &nodeState();

  NodeState const &nodeState() const;

  QRectF boundingRect() const override;

  /// Visits all attached connections and corrects
  /// their corresponding end points.
  virtual void moveConnections() const;

  virtual void
  reactToPossibleConnection(PortType, NodeDataType, QPointF const &scenePoint);

  virtual void resetReactionToConnection();

  enum { Node = UserType + 1, Frame };

  int type() const override { return Node; }

  void lock(bool locked);

  /**\brief if item collides with frame - set frame as parent
   */
  void checkParent();

protected:
  void paint(QPainter *                      painter,
             QStyleOptionGraphicsItem const *option,
             QWidget *                       widget = nullptr) override;

  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &   value) override;

  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

  void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
  void embedQWidget();

protected:
  FlowScene &_scene;

  NodeIndex _nodeIndex;

  NodeGeometry _geometry;

  NodeState _state;

  bool _locked;

  // either nullptr or owned by parent QGraphicsItem
  QGraphicsProxyWidget *_proxyWidget;
};
} // namespace QtNodes

#endif
