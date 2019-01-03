// NodeComposite.cpp

#include "NodeComposite.hpp"
#include "FlowScene.hpp"
#include "FlowSceneModel.hpp"
#include "NodePainter.hpp"
#include <QCursor>
#include <QGraphicsEffect>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

namespace QtNodes {
NodeComposite::NodeComposite(FlowScene &scene, const NodeIndex &nodeIndex)
    : scene_{scene}
    , nodeIndex_{nodeIndex}
    , nodeState_{nodeIndex}
    , locked_{false} {
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  auto const nodeStyle = NodeStyle{};
  {
    auto effect = new QGraphicsDropShadowEffect;
    effect->setOffset(4, 4);
    effect->setBlurRadius(20);
    effect->setColor(nodeStyle.ShadowColor);

    setGraphicsEffect(effect);
  }

  setOpacity(nodeStyle.Opacity);

  setAcceptHoverEvents(true);

  setZValue(0.);
}

int NodeComposite::type() const {
  return NodeComposite::Composite;
}

bool NodeComposite::interractWith(NodeComposite *) {
  return false;
}

void NodeComposite::reactToPossibleConnection(PortType,
                                              NodeDataType,
                                              const QPointF &) {}

void NodeComposite::resetReactionToConnection() {}

QRectF NodeComposite::boundingRect() const {
  return geometry().boundingRect();
}

NodeIndex NodeComposite::nodeIndex() const {
  return nodeIndex_;
}

FlowScene &NodeComposite::flowScene() {
  return scene_;
}

const FlowScene &NodeComposite::flowScene() const {
  return scene_;
}

NodeState &NodeComposite::nodeState() {
  return nodeState_;
}

const NodeState &NodeComposite::nodeState() const {
  return nodeState_;
}

void NodeComposite::lock(bool locked) {
  locked_ = locked;
  setFlag(QGraphicsItem::ItemIsMovable, !locked);
  setFlag(QGraphicsItem::ItemIsFocusable, !locked);
  setFlag(QGraphicsItem::ItemIsSelectable, !locked);
}

bool NodeComposite::isLocked() const {
  return locked_;
}

void NodeComposite::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (isLocked()) {
    event->ignore();
    return;
  }

  if (!isSelected() && !(event->modifiers() & Qt::ControlModifier)) {
    flowScene().clearSelection();
  }

  if (geometry().resizeRect().contains(
          QPoint(event->pos().x(), event->pos().y()))) {
    nodeState().setResizing(true);
  }
}

void NodeComposite::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (nodeState().resizing()) {
    auto diff = event->pos() - event->lastPos();

    geometry().setWidth(geometry().width() + diff.x());
    geometry().setHeight(geometry().height() + diff.y());

    event->accept();
  } else {
    QGraphicsObject::mouseMoveEvent(event);

    event->ignore();
  }
}

void NodeComposite::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  nodeState().setResizing(false);
  QGraphicsObject::mouseReleaseEvent(event);
}

void NodeComposite::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  for (auto &item : collidingItems()) {
    if (item->parentItem() == this->parentItem() &&
        item->type() == this->type()) {
      item->stackBefore(this);
    }
  }

  geometry().setHovered(true);
  update();
  event->accept();
}

void NodeComposite::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  auto pos = event->pos();

  if (geometry().resizeRect().contains(QPoint(pos.x(), pos.y()))) {
    setCursor(QCursor(Qt::SizeFDiagCursor));
  } else {
    setCursor(QCursor());
  }

  event->accept();
}

void NodeComposite::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  geometry().setHovered(false);
  update();
  event->accept();
}

void NodeComposite::setSize(QSize size) {
  geometry().setWidth(size.width());
  geometry().setHeight(size.height());
}

QSize NodeComposite::size() const {
  return QSize(geometry().width(), geometry().height());
}

}; // namespace QtNodes
