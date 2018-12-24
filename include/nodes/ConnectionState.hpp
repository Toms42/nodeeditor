#pragma once

#include "PortType.hpp"
#include <QtCore/QUuid>

class QPointF;

namespace QtNodes {

class NodeGraphicsObject;

/// Stores currently draggind end.
/// Remembers last hovered Node.
class ConnectionState {
public:
  ConnectionState(PortType port = PortType::None) : _requiredPort(port) {}

  ConnectionState(const ConnectionState &) = delete;
  ConnectionState operator=(const ConnectionState &) = delete;

  ~ConnectionState();

public:
  void setRequiredPort(PortType end);

  PortType requiredPort() const;

  bool requiresPort() const;

  void setNoRequiredPort();

public:
  void interactWithNode(NodeGraphicsObject *node);

  void setLastHoveredNode(NodeGraphicsObject *node);

  NodeGraphicsObject *lastHoveredNode() const;

  void resetLastHoveredNode();

private:
  PortType _requiredPort;

  NodeGraphicsObject *_lastHoveredNode{nullptr};
};
} // namespace QtNodes
