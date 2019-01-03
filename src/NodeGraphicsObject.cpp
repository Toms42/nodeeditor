#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "FlowScene.hpp"
#include "FlowSceneModel.hpp"
#include "Node.hpp"
#include "NodeConnectionInteraction.hpp"
#include "NodeIndex.hpp"
#include "NodePainter.hpp"
#include "StyleCollection.hpp"
#include <QtWidgets/QGraphicsEffect>
#include <QtWidgets/QtWidgets>
#include <cstdlib>
#include <iostream>
#include <utility>

using QtNodes::ConnectionPolicy;
using QtNodes::FlowScene;
using QtNodes::NodeGeometry;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeIndex;
using QtNodes::NodeState;

NodeGraphicsObject::NodeGraphicsObject(FlowScene &scene, NodeIndex const &node)
    : NodeComposite{scene, node}
    , proxyWidget_{new QGraphicsProxyWidget(this)}
    , geometry_{*this} {
  embedQWidget();

  connect(reinterpret_cast<class Node *>(node.internalPointer()),
          &Node::widgetChanged,
          this,
          &NodeGraphicsObject::embedQWidget);
}

NodeGraphicsObject::~NodeGraphicsObject() {}

int NodeGraphicsObject::type() const {
  return NodeComposite::Node;
}

void NodeGraphicsObject::moveConnections() const {
  for (PortType portType : {PortType::In, PortType::Out}) {
    for (auto const &connections : nodeState().getEntries(portType)) {
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

  return NodeComposite::itemChange(change, value);
}

void NodeGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  NodeComposite::mousePressEvent(event);

  for (PortType portToCheck : {PortType::In, PortType::Out}) {
    // TODO do not pass sceneTransform
    PortIndex portIndex = geometry_.checkHitScenePoint(
        portToCheck, event->scenePos(), sceneTransform());
    if (portIndex != INVALID) {
      std::vector<ConnectionGraphicsObject *> connections =
          nodeState().connections(portToCheck, portIndex);

      // start dragging existing connection
      if (!connections.empty() &&
          reinterpret_cast<class Node *>(nodeIndex().internalPointer())
                  ->nodePortConnectionPolicy(portToCheck, portIndex) ==
              ConnectionPolicy::One) {
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
        emit flowScene().removeConnection(con->node(PortType::Out),
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

void NodeGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  NodeComposite::mouseReleaseEvent(event);
  for (const auto &selected : flowScene().selectedItems()) {
    bool success{false};
    if (auto selectedComposite = dynamic_cast<NodeComposite *>(selected);
        selectedComposite) {
      for (auto &i : selectedComposite->collidingItems(
               Qt::ItemSelectionMode::ContainsItemShape)) {
        if (auto *item = dynamic_cast<NodeComposite *>(i); item) {
          if (item->interractWith(selectedComposite)) {
            success = true;
            break;
          }
        }
      }
      if (auto parent = selectedComposite->parentItem(); !success && parent) {
        selectedComposite->setParentItem(nullptr);
        selectedComposite->setPos(parent->mapToScene(selectedComposite->pos()));
      }
    }
  }

  event->accept();
}

void NodeGraphicsObject::mouseDoubleClickEvent(
    QGraphicsSceneMouseEvent *event) {
  QGraphicsItem::mouseDoubleClickEvent(event);
}

void NodeGraphicsObject::contextMenuEvent(
    QGraphicsSceneContextMenuEvent *event) {
  QGraphicsItem::contextMenuEvent(event);
}

void NodeGraphicsObject::embedQWidget() {
  if (!nodeIndex().internalPointer()) {
    return;
  }
  if (auto w = reinterpret_cast<class Node *>(nodeIndex().internalPointer())
                   ->nodeWidget()) {
    proxyWidget_->setWidget(w);

    proxyWidget_->setPreferredWidth(5);

    geometry().recalculateSize();

    proxyWidget_->setPos(geometry_.widgetPosition());

    proxyWidget_->setOpacity(1.0);
    proxyWidget_->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
  }
}

QGraphicsProxyWidget *NodeGraphicsObject::proxyWidget() {
  return proxyWidget_;
}

const QGraphicsProxyWidget *NodeGraphicsObject::proxyWidget() const {
  return proxyWidget_;
}

QtNodes::CompositeGeometry &NodeGraphicsObject::geometry() {
  return geometry_;
}

const QtNodes::CompositeGeometry &NodeGraphicsObject::geometry() const {
  return geometry_;
}

void NodeGraphicsObject::paint(QPainter *                      painter,
                               QStyleOptionGraphicsItem const *option,
                               QWidget *) {
  painter->setClipRect(option->exposedRect);

  NodePainter::paint(painter, *this);
}
