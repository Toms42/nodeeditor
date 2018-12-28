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

  auto const nodeStyle = scene_.model()->nodeStyle(nodeIndex_);
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

  // connect to the move signals
  // auto onMoveSlot = [this] {
  //  // ask the model to move it
  //  if (!flowScene().model()->moveNode(nodeIndex_, scenePos())) {
  //    // set the location back
  //    setPos(flowScene().model()->nodeLocation(nodeIndex_));
  //  }
  //};
  // connect(this, &QGraphicsObject::xChanged, this, onMoveSlot);
  // connect(this, &QGraphicsObject::yChanged, this, onMoveSlot);
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
    //    auto oldSize = w->size();

    //    oldSize += QSize(diff.x(), diff.y());
    //    if (auto sizeHint = w->minimumSizeHint();
    //        oldSize.height() < sizeHint.height() ||
    //        oldSize.width() < sizeHint.width()) {
    //      return;
    //    }

    //    w->setFixedSize(oldSize);

    // We have to recalculate size before set widgetPosition in proxyWidget
    geometry().recalculateSize();

    //    proxyWidget()->setMinimumSize(oldSize);
    //    proxyWidget()->setMaximumSize(oldSize);
    //    proxyWidget()->setPos(geometry().widgetPosition());

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
  switch (this->type()) {
  case NodeComposite::Node:
    // bring all the colliding nodes to background
    for (auto &item : collidingItems()) {
      if (item->type() == NodeComposite::Node) {
        item->setZValue(0.0);
      }
    }
    // bring this node forward
    setZValue(1.0);

    geometry().setHovered(true);
    update();
    event->accept();
    break;
  case NodeComposite::Frame:
    for (auto &item : collidingItems()) {
      if (item->type() == NodeComposite::Frame) {
        item->setZValue(-1.0);
      }
    }
    setZValue(-0.9);
    event->accept();
    break;
  default:
    break;
  }
}

void NodeComposite::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  auto pos = event->pos();

  if (/*flowScene().model()->nodeResizable(nodeIndex()) &&*/
      geometry().resizeRect().contains(QPoint(pos.x(), pos.y()))) {
    setCursor(QCursor(Qt::SizeFDiagCursor));
  } else {
    setCursor(QCursor());
  }

  event->accept();
}

void NodeComposite::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  switch (this->type()) {
  case NodeComposite::Node:
    geometry().setHovered(false);
    update();
    event->accept();
    break;
  case NodeComposite::Frame:
    setZValue(-1.0);
    event->accept();
    break;
  default:
    break;
  }
}

void NodeComposite::focusInEvent(QFocusEvent *event) {
  geometry().setHovered(true);
  update();
  event->accept();
}

void NodeComposite::focusOutEvent(QFocusEvent *event) {
  geometry().setHovered(false);
  update();
  event->accept();
}

}; // namespace QtNodes
