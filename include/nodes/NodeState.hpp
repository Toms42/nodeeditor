#ifndef NODES_NODESTATE_HPP
#define NODES_NODESTATE_HPP

#pragma once

#include "Export.hpp"
#include "PortType.hpp"
#include "memory.hpp"
#include "types.hpp"
#include <QObject>
#include <QtCore/QUuid>
#include <map>
#include <unordered_map>

namespace QtNodes {

class NodeIndex;
class ConnectionGraphicsObject;

/// Contains vectors of connected input and output connections.
/// Stores bool for reacting on hovering connections
class NODE_EDITOR_PUBLIC NodeState : public QObject {
  Q_OBJECT
public:
  enum ReactToConnectionState { REACTING, NOT_REACTING };

public:
  explicit NodeState(NodeIndex const &index);

public:
  using ConnectionPtrVec = std::vector<ConnectionGraphicsObject *>;

  /// Returns vector of connections ID.
  /// Some of them can be empty (null)
  std::map<PortIndex, ConnectionPtrVec> const &getEntries(PortType) const;

  std::map<PortIndex, ConnectionPtrVec> &getEntries(PortType);

  ConnectionPtrVec connections(PortType portType, PortIndex portIndex) const;

  void setConnection(PortType                  portType,
                     PortIndex                 portIndex,
                     ConnectionGraphicsObject &connection);

  void eraseConnection(PortType                  portType,
                       PortIndex                 portIndex,
                       ConnectionGraphicsObject &connection);

  ReactToConnectionState reaction() const;

  PortType reactingPortType() const;

  NodeDataType reactingDataType() const;

  void setReaction(ReactToConnectionState reaction,
                   PortType               reactingPortType = PortType::None,
                   NodeDataType           reactingDataType = NodeDataType());

  bool isReacting() const;

  void setResizing(bool resizing);

  bool resizing() const;

public slots:
  /**\brief add new port. Uses by model
   */
  void addPort(PortType pType, PortIndex pIndex);

  /**\brief remove port. Uses by model
   */
  void removePort(PortType pType, PortIndex pIndex);

private:
  const NodeIndex &                     nodeIndex_;
  std::map<PortIndex, ConnectionPtrVec> _inConnections;
  std::map<PortIndex, ConnectionPtrVec> _outConnections;

  ReactToConnectionState _reaction;
  PortType               _reactingPortType;
  NodeDataType           _reactingDataType;

  bool _resizing;
};
} // namespace QtNodes

#endif
