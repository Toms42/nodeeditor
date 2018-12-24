// NodeGraphicsFrame.hpp

#include "NodeGraphicsFrame.hpp"
#include "FlowSceneModel.hpp"
#include <QCursor>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

namespace QtNodes {

NodeGraphicsFrame::NodeGraphicsFrame(FlowScene &scene,
                                     const NodeIndex &nodeIndex)
    : NodeGraphicsObject{scene, nodeIndex} {
  setZValue(-1.0);
  _proxyWidget->setOpacity(0);
}

NodeGraphicsFrame::~NodeGraphicsFrame() {
  for (auto &i : childItems()) {
    i->setParentItem(nullptr);
    i->setPos(i->pos() + pos());
  }
}

int NodeGraphicsFrame::type() const { return Frame; }

void NodeGraphicsFrame::reactToPossibleConnection(PortType portType,
                                                  NodeDataType dataType,
                                                  const QPointF &scenePoint) {
  Q_UNUSED(portType);
  Q_UNUSED(dataType);
  Q_UNUSED(scenePoint);
}

void NodeGraphicsFrame::resetReactionToConnection() {}

QVariant NodeGraphicsFrame::itemChange(GraphicsItemChange change,
                                       const QVariant &value) {
  return QGraphicsItem::itemChange(change, value);
}

void NodeGraphicsFrame::embedQWidget() {
  if (auto w = flowScene().model()->nodeWidget(index())) {
    _proxyWidget = new QGraphicsProxyWidget(this);

    _proxyWidget->setWidget(w);

    _proxyWidget->setPreferredWidth(5);

    geometry().recalculateSize();

    _proxyWidget->setPos(geometry().widgetPosition());

    update();

    _proxyWidget->setOpacity(0);
    _proxyWidget->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
  }
}

void NodeGraphicsFrame::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (_locked) {
    return;
  }

  if (!isSelected() && !(event->modifiers() & Qt::ControlModifier)) {
    _scene.clearSelection();
  }

  auto pos = event->pos();

  if (geometry().resizeRect().contains(QPoint(pos.x(), pos.y()))) {
    nodeState().setResizing(true);
  }
}

void NodeGraphicsFrame::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  NodeGraphicsObject::mouseMoveEvent(event);
}

void NodeGraphicsFrame::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  nodeState().setResizing(false);
  QGraphicsObject::mouseReleaseEvent(event);
}

void NodeGraphicsFrame::moveConnections() const {}

void NodeGraphicsFrame::hoverEnterEvent(QGraphicsSceneHoverEvent *) {}

void NodeGraphicsFrame::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {}

void NodeGraphicsFrame::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  auto pos = event->pos();

  if (flowScene().model()->nodeResizable(index()) &&
      geometry().resizeRect().contains(QPoint(pos.x(), pos.y()))) {
    setCursor(QCursor(Qt::SizeFDiagCursor));
  } else {
    setCursor(QCursor());
  }

  event->accept();
}

} // namespace QtNodes
