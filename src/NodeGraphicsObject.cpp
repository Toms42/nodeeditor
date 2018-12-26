#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "FlowScene.hpp"
#include "FlowSceneModel.hpp"
#include "NodeConnectionInteraction.hpp"
#include "NodeIndex.hpp"
#include "NodePainter.hpp"
#include "StyleCollection.hpp"
#include <QtWidgets/QGraphicsEffect>
#include <QtWidgets/QtWidgets>
#include <cstdlib>
#include <utility>

using QtNodes::ConnectionPolicy;
using QtNodes::FlowScene;
using QtNodes::NodeGeometry;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeIndex;
using QtNodes::NodeState;

NodeGraphicsObject::NodeGraphicsObject(FlowScene &scene, NodeIndex const &node)
    : NodeComposite{scene, node} {
  setZValue(1.);

  // TODO here we set locatin to Node, and after get it and set position to it
  // location - it is not good
  //
  // connect to the move signals
  //
  // auto onMoveSlot =
  // [this] {
  //  // ask the model to move it
  //  if (!flowScene().model()->moveNode(index(), scenePos())) {
  //    // set the location back
  //    setPos(flowScene().model()->nodeLocation(index()));
  //    moveConnections();
  //  }
  //};
  // connect(this, &QGraphicsObject::xChanged, this, onMoveSlot);
  // connect(this, &QGraphicsObject::yChanged, this, onMoveSlot);
}

NodeGraphicsObject::~NodeGraphicsObject() {}

int NodeGraphicsObject::type() const {
  return NodeComposite::Node;
}

void NodeGraphicsObject::moveConnections() const {
  for (PortType portType : {PortType::In, PortType::Out}) {
    auto const &connectionEntries = nodeState().getEntries(portType);

    for (auto const &connections : connectionEntries) {
      for (auto &con : connections.second) {
        con->move();
      }
    }
  };
}

void NodeGraphicsObject::reactToPossibleConnection(
    PortType       reactingPortType,
    NodeDataType   reactingDataType,
    QPointF const &scenePoint) {
  QTransform const t = sceneTransform();

  QPointF p = t.inverted().map(scenePoint);

  geometry().setDraggingPosition(p);

  update();

  nodeState().setReaction(
      NodeState::REACTING, reactingPortType, std::move(reactingDataType));
}

void NodeGraphicsObject::resetReactionToConnection() {
  nodeState().setReaction(NodeState::NOT_REACTING);
  update();
}

QVariant NodeGraphicsObject::itemChange(GraphicsItemChange change,
                                        const QVariant &   value) {
  // here we use scene position, because item can have a parent
  if (change == ItemScenePositionHasChanged && scene()) {
    moveConnections();
  }

  return QGraphicsItem::itemChange(change, value);
}

void NodeGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  NodeComposite::mousePressEvent(event);

  for (PortType portToCheck : {PortType::In, PortType::Out}) {
    // TODO do not pass sceneTransform
    PortIndex portIndex = geometry().checkHitScenePoint(
        portToCheck, event->scenePos(), sceneTransform());
    if (portIndex != INVALID) {
      std::vector<ConnectionGraphicsObject *> connections =
          nodeState().connections(portToCheck, portIndex);

      // start dragging existing connection
      if (!connections.empty() &&
          flowScene().model()->nodePortConnectionPolicy(
              nodeIndex(), portIndex, portToCheck) == ConnectionPolicy::One) {
        Q_ASSERT(connections.size() == 1);

        auto con = *connections.begin();

        // start connecting anew, except start with the port that this
        // connection was already connected to
        Q_ASSERT(flowScene().temporaryConn() == nullptr);
        if (portToCheck == PortType::In) {
          flowScene().setTemporaryConn(
              new ConnectionGraphicsObject(con->node(PortType::Out),
                                           con->portIndex(PortType::Out),
                                           NodeIndex{},
                                           INVALID,
                                           flowScene()));
          flowScene().temporaryConn()->geometry().setEndPoint(
              PortType::In, event->scenePos());
        } else {
          flowScene().setTemporaryConn(
              new ConnectionGraphicsObject(NodeIndex{},
                                           INVALID,
                                           con->node(PortType::In),
                                           con->portIndex(PortType::In),
                                           flowScene()));
          flowScene().temporaryConn()->geometry().setEndPoint(
              PortType::Out, event->scenePos());
        }
        flowScene().temporaryConn()->grabMouse();

        // past create new connection by existing, we remove already existing
        flowScene().model()->removeConnection(con->node(PortType::Out),
                                              con->portIndex(PortType::Out),
                                              con->node(PortType::In),
                                              con->portIndex(PortType::In));

      } else // initialize new Connection
      {
        if (portToCheck == PortType::In) {
          Q_ASSERT(flowScene().temporaryConn() == nullptr);
          flowScene().setTemporaryConn(new ConnectionGraphicsObject(
              NodeIndex{}, INVALID, nodeIndex(), portIndex, flowScene()));
          flowScene().temporaryConn()->geometry().setEndPoint(
              PortType::Out, event->scenePos());
        } else {
          Q_ASSERT(flowScene().temporaryConn() == nullptr);
          flowScene().setTemporaryConn(new ConnectionGraphicsObject(
              nodeIndex(), portIndex, NodeIndex{}, INVALID, flowScene()));
          flowScene().temporaryConn()->geometry().setEndPoint(
              PortType::In, event->scenePos());
        }

        flowScene().temporaryConn()->grabMouse();
      }
    }
  }
}

void NodeGraphicsObject::mouseDoubleClickEvent(
    QGraphicsSceneMouseEvent *event) {
  QGraphicsItem::mouseDoubleClickEvent(event);

  flowScene().model()->nodeDoubleClicked(nodeIndex(), event->screenPos());
}

void NodeGraphicsObject::contextMenuEvent(
    QGraphicsSceneContextMenuEvent *event) {
  QGraphicsItem::contextMenuEvent(event);

  flowScene().model()->nodeContextMenu(nodeIndex(), event->screenPos());
}
