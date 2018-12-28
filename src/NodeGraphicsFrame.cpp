// NodeGraphicsFrame.hpp

#include "NodeGraphicsFrame.hpp"
#include "FlowSceneModel.hpp"
#include "FramePainter.hpp"
#include <QCursor>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace QtNodes {

NodeGraphicsFrame::NodeGraphicsFrame(FlowScene &      scene,
                                     const NodeIndex &nodeIndex)
    : NodeComposite{scene, nodeIndex}
    , geometry_{*this} {
  setZValue(-1.0);
}

NodeGraphicsFrame::~NodeGraphicsFrame() {
  // frame can't tell to model about delete items - it have to do scene
  unloadAllChilds();
}

int NodeGraphicsFrame::type() const {
  return Frame;
}

bool NodeGraphicsFrame::interractWith(NodeComposite *obj) {
  if (obj) {
    if (this != obj->parentItem()) {
      auto childPos = obj->scenePos();
      obj->setParentItem(this);
      obj->setPos(mapFromScene(childPos));
    }
    return true;
  }
  return false;
}

void NodeGraphicsFrame::unloadAllChilds() {
  for (auto &i : childItems()) {
    if (i->type() > QGraphicsItem::UserType) {
      i->setParentItem(nullptr);
      i->setPos(mapToScene(i->pos()));
    }
  }
}

void NodeGraphicsFrame::contextMenuEvent(
    QGraphicsSceneContextMenuEvent *event) {
  auto frameMenu    = flowScene().createContextMenu(event->scenePos());
  auto unloadAction = new QAction("Unload all Nodes from Frame", frameMenu);
  frameMenu->addAction(unloadAction);
  connect(unloadAction, &QAction::triggered, this, [this, frameMenu]() {
    unloadAllChilds();
    frameMenu->close();
  });
  frameMenu->exec(event->screenPos());
  frameMenu->deleteLater();
  event->accept();
}

CompositeGeometry &NodeGraphicsFrame::geometry() {
  return geometry_;
}

const CompositeGeometry &NodeGraphicsFrame::geometry() const {
  return geometry_;
}

void NodeGraphicsFrame::paint(QPainter *                      painter,
                              QStyleOptionGraphicsItem const *option,
                              QWidget *) {
  painter->setClipRect(option->exposedRect);

  FramePainter::paint(painter, *this);
}

void NodeGraphicsFrame::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  NodeComposite::mouseReleaseEvent(event);

  bool success{false};
  for (auto &i : collidingItems(Qt::ItemSelectionMode::ContainsItemShape)) {
    if (auto *item = dynamic_cast<NodeComposite *>(i); item) {
      if (item->interractWith(this)) {
        success = true;
        break;
      }
    }
  }
  if (auto parent = parentItem(); !success && parent) {
    setParentItem(nullptr);
    setPos(parent->mapToScene(pos()));
  }

  event->accept();
}
} // namespace QtNodes
