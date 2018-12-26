// NodeGraphicsFrame.hpp

#include "NodeGraphicsFrame.hpp"
#include "FlowSceneModel.hpp"
#include <QCursor>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>

namespace QtNodes {

NodeGraphicsFrame::NodeGraphicsFrame(FlowScene &      scene,
                                     const NodeIndex &nodeIndex)
    : NodeComposite{scene, nodeIndex} {
  setZValue(-1.0);
  proxyWidget()->setOpacity(0);
}

NodeGraphicsFrame::~NodeGraphicsFrame() {
  for (auto &i : childItems()) {
    i->setPos(mapToScene(i->pos()));
    i->setParentItem(nullptr);
  }
}

int NodeGraphicsFrame::type() const {
  return Frame;
}

bool NodeGraphicsFrame::canBeParent() const {
  return true;
}

void NodeGraphicsFrame::reactToPossibleConnection(PortType       portType,
                                                  NodeDataType   dataType,
                                                  const QPointF &scenePoint) {
  Q_UNUSED(portType);
  Q_UNUSED(dataType);
  Q_UNUSED(scenePoint);
}

void NodeGraphicsFrame::resetReactionToConnection() {}

} // namespace QtNodes
