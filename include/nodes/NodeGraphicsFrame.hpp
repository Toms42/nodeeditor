// NodeGraphicsFrame.hpp

#pragma once

#include "FlowScene.hpp"
#include "NodeGeometry.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeIndex.hpp"
#include "NodeState.hpp"

namespace QtNodes {
class NodeGraphicsFrame final : public NodeGraphicsObject {
  Q_OBJECT

public:
  explicit NodeGraphicsFrame(FlowScene &scene, const NodeIndex &nodeIndex);

  ~NodeGraphicsFrame() override;

  int type() const override;

  /**\brief void method
   */
  void reactToPossibleConnection(PortType       portType,
                                 NodeDataType   dataType,
                                 const QPointF &scenePoint) override;

  /**\brief void method
   */
  void resetReactionToConnection() override;

  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &   value) override;

  /**\brief void method
   */
  void moveConnections() const override;

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  /**\brief void method
   */
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

  /**\brief void method
   */
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

  void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
};
}; // namespace QtNodes
