// NodeComposite.hpp

#pragma once

#include "CompositeGeometry.hpp"
#include "Export.hpp"
#include "NodeIndex.hpp"
#include "NodeState.hpp"
#include "types.hpp"
#include <QGraphicsObject>

class QGraphicsProxyWidget;

namespace QtNodes {
class FlowScene;

class NODE_EDITOR_PUBLIC NodeComposite : public QGraphicsObject {
  Q_OBJECT

  friend void CompositeGeometry::prepareGeometryChange() const;

public:
  /**\brief by default setOpacity like in current style, ZValue = 0, and
   * initialize proxyWidget by widget in NodeDataModel
   */
  NodeComposite(FlowScene &scene, const NodeIndex &nodeIndex);
  ~NodeComposite() override = default;

  /**\brief for example this can be used for set parrent to @obj. By default do
   * nothing
   * \return by default - false
   */
  virtual bool interractWith(NodeComposite *obj);

  enum { Composite = UserType + 1, Frame, Node, OtherType = UserType + 20 };
  /**\brief specify type of node. Use OtherType, for specify new type
   */
  virtual int type() const override;

  /**\brief set react to connection. By default does nothing
   */
  virtual void reactToPossibleConnection(PortType       portType,
                                         NodeDataType   dataType,
                                         const QPointF &scenePoint);

  /**\brief reset reaction, which was be set. By default does nothing
   */
  virtual void resetReactionToConnection();

  QRectF boundingRect() const override;

  /**\return object, which describe geometry of current node
   */
  virtual CompositeGeometry &      geometry()       = 0;
  virtual const CompositeGeometry &geometry() const = 0;

  /**\return NodeIndex of current node
   */
  virtual NodeIndex nodeIndex() const;

  /**\return scene, part of which is current node
   */
  virtual FlowScene &      flowScene();
  virtual const FlowScene &flowScene() const;

  /**\return object, which describe state of current node
   */
  virtual NodeState &      nodeState();
  virtual const NodeState &nodeState() const;

  /**\param locked if true - lock node (unmovable etc), if false - unlock
   */
  virtual void lock(bool locked);
  virtual bool isLocked() const;

  /**\brief uses geometry for set size
   */
  virtual void  setSize(QSize size);
  virtual QSize size() const;

protected:
  /**\brief supports resizing and selecting nodes
   */
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

  /**\brief supports resizing and selecting nodes
   */
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  /**\brief set node on forward plane.
   * \warning Node have to have z value more than 0, else not have effect
   */
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

  /**\brief supports cursor change
   */
  void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

  /**\brief return node back to 0 z
   */
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

  void focusInEvent(QFocusEvent *event) override;

  void focusOutEvent(QFocusEvent *event) override;

private:
  FlowScene &scene_;
  NodeIndex  nodeIndex_;
  NodeState  nodeState_;
  bool       locked_;
};
}; // namespace QtNodes
