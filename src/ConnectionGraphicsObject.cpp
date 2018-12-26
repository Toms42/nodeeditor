#include "ConnectionGraphicsObject.hpp"
#include "ConnectionBlurEffect.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionState.hpp"
#include "FlowScene.hpp"
#include "FlowSceneModel.hpp"
#include "NodeConnectionInteraction.hpp"
#include "NodeGraphicsObject.hpp"
#include "checker.hpp"
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <iostream>

using QtNodes::ConnectionGraphicsObject;
using QtNodes::ConnectionID;
using QtNodes::FlowScene;
using QtNodes::NodeDataType;

ConnectionGraphicsObject::ConnectionGraphicsObject(NodeIndex const &leftNode,
                                                   PortIndex leftPortIndex,
                                                   NodeIndex const &rightNode,
                                                   PortIndex  rightPortIndex,
                                                   FlowScene &scene)
    : _scene{scene}
    , _geometry(*this)
    , _state(leftNode.isValid()
                 ? (rightNode.isValid() ? PortType::None : PortType::In)
                 : PortType::Out)
    , _leftNode{leftNode}
    , _rightNode{rightNode}
    , _leftPortIndex{leftPortIndex}
    , _rightPortIndex{rightPortIndex} {
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setAcceptHoverEvents(true);

  // addGraphicsEffect();

  setZValue(-0.5);

  // initialize the end points
  if (leftNode.isValid()) {
    auto ngo = _scene.nodeComposite(leftNode);
    Q_ASSERT(ngo != nullptr);

    geometry().moveEndPoint(
        PortType::Out,
        ngo->geometry().portScenePosition(
            leftPortIndex, PortType::Out, ngo->sceneTransform()));
  }
  if (rightNode.isValid()) {
    auto ngo = _scene.nodeComposite(rightNode);
    Q_ASSERT(ngo != nullptr);

    geometry().moveEndPoint(
        PortType::In,
        ngo->geometry().portScenePosition(
            rightPortIndex, PortType::In, ngo->sceneTransform()));
  }
}

ConnectionGraphicsObject::~ConnectionGraphicsObject() {}

int ConnectionGraphicsObject::type() const {
  return Connection;
}

QtNodes::NodeIndex ConnectionGraphicsObject::node(PortType pType) const {
  return pType == PortType::In ? _rightNode : _leftNode;
}
QtNodes::PortIndex ConnectionGraphicsObject::portIndex(PortType pType) const {
  return pType == PortType::In ? _rightPortIndex : _leftPortIndex;
}

FlowScene &ConnectionGraphicsObject::flowScene() const {
  return _scene;
}

QtNodes::ConnectionGeometry const &ConnectionGraphicsObject::geometry() const {
  return _geometry;
}

QtNodes::ConnectionGeometry &ConnectionGraphicsObject::geometry() {
  return _geometry;
}

QtNodes::ConnectionState const &ConnectionGraphicsObject::state() const {
  return _state;
}

QtNodes::ConnectionState &ConnectionGraphicsObject::state() {
  return _state;
}

QRectF ConnectionGraphicsObject::boundingRect() const {
  return _geometry.boundingRect();
}

QPainterPath ConnectionGraphicsObject::shape() const {
#ifdef DEBUG_DRAWING

  // QPainterPath path;

  // path.addRect(boundingRect());
  // return path;

#else

  return ConnectionPainter::getPainterStroke(_geometry);

#endif
}

ConnectionID ConnectionGraphicsObject::id() const {
  ConnectionID ret;

  ret.lNodeID = _leftNode.id();
  ret.rNodeID = _rightNode.id();
  ret.lPortID = _leftPortIndex;
  ret.rPortID = _rightPortIndex;

  return ret;
}

NodeDataType ConnectionGraphicsObject::dataType(PortType ty) const {
  // get a valid node
  auto n = node(ty);
  Q_ASSERT(n.isValid());

  return _scene.model()->nodePortDataType(n, portIndex(ty), ty);
}

void ConnectionGraphicsObject::move() {
  for (PortType portType : {PortType::In, PortType::Out}) {
    auto nodeIndex = node(portType);
    if (nodeIndex.isValid()) {
      // here we dereference pointer, which can be nullptr! So we first check
      // this
      auto temp = _scene.nodeComposite(nodeIndex);
      if (temp) {
        auto const &nodeGraphics = *temp;

        auto const &nodeGeom = nodeGraphics.geometry();

        QPointF scenePos = nodeGeom.portScenePosition(
            portIndex(portType), portType, nodeGraphics.sceneTransform());

        QTransform sceneTransform = this->sceneTransform();

        QPointF connectionPos = sceneTransform.inverted().map(scenePos);

        geometry().setEndPoint(portType, connectionPos);
      } else {
        GET_INFO();
      }
    }
  }
}

void ConnectionGraphicsObject::lock(bool locked) {
  setFlag(QGraphicsItem::ItemIsMovable, !locked);
  setFlag(QGraphicsItem::ItemIsFocusable, !locked);
  setFlag(QGraphicsItem::ItemIsSelectable, !locked);
}

void ConnectionGraphicsObject::paint(QPainter *                      painter,
                                     QStyleOptionGraphicsItem const *option,
                                     QWidget *) {
  painter->setClipRect(option->exposedRect);

  ConnectionPainter::paint(painter, *this);
}

void ConnectionGraphicsObject::mousePressEvent(
    QGraphicsSceneMouseEvent *event) {
  QGraphicsItem::mousePressEvent(event);
  // event->ignore();
}

void ConnectionGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  auto view = reinterpret_cast<QGraphicsView *>(event->widget());
  auto node = locateNodeAt(event->scenePos(), _scene, view->transform());

  // TODO I don't see any reason for do this
  // state().interactWithNode(node);
  if (node) {
    node->reactToPossibleConnection(
        state().requiredPort(),
        dataType(oppositePort(state().requiredPort())),
        event->scenePos());
  }

  //-------------------

  QPointF offset = event->pos() - event->lastPos();

  auto requiredPort = _state.requiredPort();
  if (requiredPort != PortType::None) {
    _geometry.moveEndPoint(requiredPort, offset);
  }

  event->accept();
}

void ConnectionGraphicsObject::mouseReleaseEvent(
    QGraphicsSceneMouseEvent *event) {
  ungrabMouse();
  event->accept();

  try {
    auto node = locateNodeAt(
        event->scenePos(), _scene, _scene.views().at(0)->transform());
    if (!node) {
      if (state().requiresPort()) {
        Q_ASSERT(this == _scene.temporaryConn());
        // remove this from the scene
        _scene.setTemporaryConn(nullptr);
        deleteLater();
      }
      return;
    }

    if (!state().requiresPort()) {
      return;
    }

    NodeConnectionInteraction interaction(node->nodeIndex(), *this);

    if (interaction.tryConnect()) {
      node->resetReactionToConnection();
      Q_ASSERT(this == _scene.temporaryConn());

      _scene.setTemporaryConn(nullptr);
      deleteLater();
    } else if (state().requiresPort()) {
      Q_ASSERT(this == _scene.temporaryConn());
      _scene.setTemporaryConn(nullptr);
      deleteLater();
    }
  } catch (std::out_of_range &) {
    GET_INFO();
  }
}

void ConnectionGraphicsObject::hoverEnterEvent(
    QGraphicsSceneHoverEvent *event) {
  geometry().setHovered(true);

  update();

  flowScene().model()->connectionHovered(_leftNode,
                                         _leftPortIndex,
                                         _rightNode,
                                         _rightPortIndex,
                                         event->screenPos(),
                                         true);

  event->accept();
}

void ConnectionGraphicsObject::hoverLeaveEvent(
    QGraphicsSceneHoverEvent *event) {
  geometry().setHovered(false);

  update();
  flowScene().model()->connectionHovered(_leftNode,
                                         _leftPortIndex,
                                         _rightNode,
                                         _rightPortIndex,
                                         event->screenPos(),
                                         false);
  event->accept();
}

void ConnectionGraphicsObject::addGraphicsEffect() {
  auto effect = new QGraphicsBlurEffect;

  effect->setBlurRadius(5);
  setGraphicsEffect(effect);

  // auto effect = new QGraphicsDropShadowEffect;
  // auto effect = new ConnectionBlurEffect(this);
  // effect->setOffset(4, 4);
  // effect->setColor(QColor(Qt::gray).darker(800));
}
