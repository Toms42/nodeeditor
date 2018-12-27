// NodeGraphicsFrame.hpp

#include "NodeGraphicsFrame.hpp"
#include "FlowSceneModel.hpp"
#include <QCursor>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

namespace QtNodes {

NodeGraphicsFrame::NodeGraphicsFrame(FlowScene &      scene,
                                     const NodeIndex &nodeIndex)
    : NodeComposite{scene, nodeIndex} {
  setZValue(-1.0);
  proxyWidget()->setOpacity(0);
}

NodeGraphicsFrame::~NodeGraphicsFrame() {
  for (auto &i : childItems()) {
    delete i;
  }
}

int NodeGraphicsFrame::type() const {
  return Frame;
}

bool NodeGraphicsFrame::canBeParent() const {
  return true;
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
  connect(unloadAction, &QAction::triggered, this, [=]() {
    unloadAllChilds();
    frameMenu->close();
  });
  frameMenu->exec(event->screenPos());
  frameMenu->deleteLater();
  event->accept();
}

} // namespace QtNodes
