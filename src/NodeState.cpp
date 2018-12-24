#include "NodeState.hpp"
#include "Connection.hpp"
#include "FlowSceneModel.hpp"
#include "Node.hpp"
#include "NodeIndex.hpp"
#include "QUuidStdHash.hpp"
#include "checker.hpp"

using QtNodes::NodeDataType;
using QtNodes::NodeState;
using QtNodes::PortIndex;
using QtNodes::PortType;

NodeState::NodeState(NodeIndex const &index)
    : nodeIndex_{index}
    , _reaction(NOT_REACTING)
    , _reactingPortType(PortType::None)
    , _resizing(false) {
  for (auto &i : index.model()->nodePortIndexes(index, PortType::In)) {
    _inConnections.insert(std::pair(i, ConnectionPtrVec{}));
  }
  for (auto &i : index.model()->nodePortIndexes(index, PortType::Out)) {
    _outConnections.insert(std::pair(i, ConnectionPtrVec{}));
  }

  Node *node = reinterpret_cast<Node *>(nodeIndex_.internalPointer());

  connect(node, &QtNodes::Node::portAdded, this, &QtNodes::NodeState::addPort);

  connect(
      node, &QtNodes::Node::portRemoved, this, &QtNodes::NodeState::removePort);
}

void NodeState::addPort(PortType pType, PortIndex pIndex) {
  switch (pType) {
  case PortType::In:
    if (!_inConnections.insert(std::pair(pIndex, ConnectionPtrVec())).second) {
      GET_INFO();
    }
    break;
  case PortType::Out:
    if (!_outConnections.insert(std::pair(pIndex, ConnectionPtrVec())).second) {
      GET_INFO();
    }
    break;
  default:
    break;
  }
}

void NodeState::removePort(PortType pType, PortIndex pIndex) {
  switch (pType) {
  case PortType::In:
    if (!_inConnections.erase(pIndex)) {
      GET_INFO();
    }
    break;
  case PortType::Out:
    if (!_outConnections.erase(pIndex)) {
      GET_INFO();
    }
    break;
  default:
    break;
  }
}

std::map<PortIndex, NodeState::ConnectionPtrVec> const &
NodeState::getEntries(PortType portType) const {
  if (portType == PortType::In)
    return _inConnections;
  else
    return _outConnections;
}

std::map<PortIndex, NodeState::ConnectionPtrVec> &
NodeState::getEntries(PortType portType) {
  if (portType == PortType::In)
    return _inConnections;
  else
    return _outConnections;
}

NodeState::ConnectionPtrVec NodeState::connections(PortType  portType,
                                                   PortIndex portIndex) const {
  auto const &connections = getEntries(portType);

  try {
    return connections.at(portIndex);
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

void NodeState::setConnection(PortType                  portType,
                              PortIndex                 portIndex,
                              ConnectionGraphicsObject &connection) {
  auto &connections = getEntries(portType);

  try {
    connections.at(portIndex).push_back(&connection);
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

void NodeState::eraseConnection(PortType                  portType,
                                PortIndex                 portIndex,
                                ConnectionGraphicsObject &conn) {
  try {
    auto &ptrSet = getEntries(portType).at(portIndex);
    auto  iter   = std::find(ptrSet.begin(), ptrSet.end(), &conn);
    if (iter != ptrSet.end()) {
      ptrSet.erase(iter);
    }
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

NodeState::ReactToConnectionState NodeState::reaction() const {
  return _reaction;
}

PortType NodeState::reactingPortType() const {
  return _reactingPortType;
}

NodeDataType NodeState::reactingDataType() const {
  return _reactingDataType;
}

void NodeState::setReaction(ReactToConnectionState reaction,
                            PortType               reactingPortType,
                            NodeDataType           reactingDataType) {
  _reaction = reaction;

  _reactingPortType = reactingPortType;

  _reactingDataType = std::move(reactingDataType);
}

bool NodeState::isReacting() const {
  return _reaction == REACTING;
}

void NodeState::setResizing(bool resizing) {
  _resizing = resizing;
}

bool NodeState::resizing() const {
  return _resizing;
}
