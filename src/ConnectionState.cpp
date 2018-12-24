#include "ConnectionState.hpp"
#include "FlowScene.hpp"
#include "NodeGraphicsObject.hpp"
#include <QtCore/QPointF>

using QtNodes::ConnectionState;
using QtNodes::NodeGraphicsObject;

ConnectionState::~ConnectionState() { resetLastHoveredNode(); }

void ConnectionState::interactWithNode(NodeGraphicsObject *node) {
  if (node) {
    _lastHoveredNode = node;
  } else {
    resetLastHoveredNode();
  }
}

void ConnectionState::setLastHoveredNode(NodeGraphicsObject *node) {
  _lastHoveredNode = node;
}

void ConnectionState::resetLastHoveredNode() {
  if (_lastHoveredNode)
    _lastHoveredNode->resetReactionToConnection();

  _lastHoveredNode = nullptr;
}

void ConnectionState::setRequiredPort(PortType end) { _requiredPort = end; }

QtNodes::PortType ConnectionState::requiredPort() const {
  return _requiredPort;
}

bool ConnectionState::requiresPort() const {
  return _requiredPort != PortType::None;
}

void ConnectionState::setNoRequiredPort() { _requiredPort = PortType::None; }

NodeGraphicsObject *ConnectionState::lastHoveredNode() const {
  return _lastHoveredNode;
}
