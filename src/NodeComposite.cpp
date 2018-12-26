// NodeComposite.cpp

#include "NodeComposite.hpp"
#include "FlowScene.hpp"
#include "FlowSceneModel.hpp"
#include "NodePainter.hpp"
#include <QGraphicsEffect>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace QtNodes {
NodeComposite::NodeComposite(FlowScene &scene, const NodeIndex &nodeIndex)
    : scene_{scene}
    , nodeIndex_{nodeIndex}
    , geometry_{nodeIndex, *this}
    , proxyWidget_{nullptr}
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

  embedQWidget();
}

int NodeComposite::type() const {
  return NodeComposite::Composite;
}

bool NodeComposite::canBeParent() const {
  return false;
}

bool NodeComposite::canBeChild() const {
  return true;
}

void NodeComposite::reactToPossibleConnection(PortType,
                                              NodeDataType,
                                              const QPointF &) {}

void NodeComposite::resetReactionToConnection() {}

QRectF NodeComposite::boundingRect() const {
  return geometry_.boundingRect();
}

NodeGeometry &NodeComposite::geometry() {
  return geometry_;
}

const NodeGeometry &NodeComposite::geometry() const {
  return geometry_;
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

void NodeComposite::paint(QPainter *                      painter,
                          QStyleOptionGraphicsItem const *option,
                          QWidget *) {
  painter->setClipRect(option->exposedRect);

  NodePainter::paint(painter, *this);
}

void NodeComposite::embedQWidget() {
  if (auto w = scene_.model()->nodeWidget(nodeIndex_)) {
    proxyWidget_ = new QGraphicsProxyWidget(this);

    proxyWidget_->setWidget(w);

    proxyWidget_->setPreferredWidth(5);

    geometry_.recalculateSize();

    proxyWidget_->setPos(geometry().widgetPosition());

    proxyWidget_->setOpacity(1.0);
    proxyWidget_->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
  }
}

QGraphicsProxyWidget *NodeComposite::proxyWidget() {
  return proxyWidget_;
}

const QGraphicsProxyWidget *NodeComposite::proxyWidget() const {
  return proxyWidget_;
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

    if (auto w = flowScene().model()->nodeWidget(nodeIndex())) {
      auto oldSize = w->size();

      oldSize += QSize(diff.x(), diff.y());
      if (auto sizeHint = w->minimumSizeHint();
          oldSize.height() < sizeHint.height() ||
          oldSize.width() < sizeHint.width()) {
        return;
      }

      w->setFixedSize(oldSize);

      // We have to recalculdate size before set widgetPosition in proxyWidget
      geometry().recalculateSize();

      proxyWidget()->setMinimumSize(oldSize);
      proxyWidget()->setMaximumSize(oldSize);
      proxyWidget()->setPos(geometry().widgetPosition());

      event->accept();
    }
  } else {
    QGraphicsObject::mouseMoveEvent(event);

    event->ignore();
  }
}

void NodeComposite::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  nodeState().setResizing(false);
  bool success{false};
  if (canBeChild()) {
    for (auto &i : collidingItems(Qt::ItemSelectionMode::ContainsItemShape)) {
      if (auto *item = dynamic_cast<NodeComposite *>(i);
          item && item->canBeParent()) {
        if (i != parentItem()) {
          auto position = scenePos();
          setParentItem(i);
          setPos(i->mapFromScene(position));
        }
        success = true;
        break;
      }
    }
    if (!success && parentItem()) {
      setPos(parentItem()->mapToScene(pos()));
      setParentItem(nullptr);
    }
  }
  QGraphicsObject::mouseReleaseEvent(event);
}

void NodeComposite::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  if (this->zValue() > 0) {
    // bring all the colliding nodes to background
    QList<QGraphicsItem *> overlapItems = collidingItems();

    for (QGraphicsItem *item : overlapItems) {
      if (item->zValue() > 0.0) {
        item->setZValue(0.0);
      }
    }
    // bring this node forward
    setZValue(1.0);

    geometry().setHovered(true);
    update();
    flowScene().model()->nodeHovered(nodeIndex(), event->screenPos(), true);
    event->accept();
  }
}

void NodeComposite::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  auto pos = event->pos();

  if (flowScene().model()->nodeResizable(nodeIndex()) &&
      geometry().resizeRect().contains(QPoint(pos.x(), pos.y()))) {
    setCursor(QCursor(Qt::SizeFDiagCursor));
  } else {
    setCursor(QCursor());
  }

  event->accept();
}

void NodeComposite::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  if (zValue() > 0) {
    geometry().setHovered(false);
    update();
    flowScene().model()->nodeHovered(nodeIndex(), event->screenPos(), false);
    event->accept();
  }
}

}; // namespace QtNodes
