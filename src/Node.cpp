#include "Node.hpp"
#include "Connection.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "FlowScene.hpp"
#include "NodeDataModel.hpp"
#include "NodeGraphicsObject.hpp"
#include "checker.hpp"
#include <QJsonArray>
#include <QtCore/QObject>
#include <utility>

using QtNodes::Connection;
using QtNodes::Node;

Node::Node(std::unique_ptr<NodeDataModel> &&nodeImp, QUuid const &id)
    : _uid(id)
    , _nodeDataModel(std::move(nodeImp)) {
  // propagate data: model => node
  connect(_nodeDataModel.get(),
          &NodeDataModel::dataUpdated,
          this,
          &Node::onDataUpdated);

  connect(_nodeDataModel.get(),
          &NodeDataModel::widgetChanged,
          this,
          &Node::widgetChanged);

  for (const auto &i : _nodeDataModel->inPorts_) {
    _inConnections.insert(
        std::pair(i.first, std::list<std::shared_ptr<Connection>>()));
  }
  for (const auto &i : _nodeDataModel->outPorts_) {
    _outConnections.insert(
        std::pair(i.first, std::list<std::shared_ptr<Connection>>()));
  }
}

Node::~Node() = default;

QWidget *Node::nodeWidget() const {
  return _nodeDataModel->embeddedWidget();
}

QtNodes::ConnectionPolicy
Node::nodePortConnectionPolicy(PortType type, PortIndex index) const {
  switch (type) {
  case PortType::In:
    return ConnectionPolicy::One;
    break;
  case PortType::Out:
    return _nodeDataModel->portOutConnectionPolicy(index);
    break;
  default:
    break;
  }
  // TODO I think this is not right, this shuld return error in this case
  return ConnectionPolicy::Many;
}

QtNodes::NodeDataType Node::nodePortDataType(PortType  type,
                                             PortIndex index) const {
  return _nodeDataModel->dataType(type, index);
}

QString Node::nodeCaption() const {
  return _nodeDataModel->name();
}

QString Node::nodePortCaption(PortType type, PortIndex index) const {
  return _nodeDataModel->portCaption(type, index);
}

QtNodes::NodeValidationState Node::nodeValidationState() const {
  return _nodeDataModel->validationState();
}

QString Node::nodeValidationMessage() const {
  return _nodeDataModel->validationMessage();
}

QtNodes::NodePainterDelegate *Node::nodePainterDelegate() const {
  return _nodeDataModel->painterDelegate();
}

bool Node::addPort(PortType pType, PortIndex pIndex) {
  switch (pType) {
  case PortType::In:
    if (_inConnections
            .insert(std::pair(pIndex, std::list<std::shared_ptr<Connection>>()))
            .second) {
      emit portAdded(pType, pIndex);
      return true;
    } else {
      GET_INFO();
    }
    break;
  case PortType::Out:
    if (_outConnections
            .insert(std::pair(pIndex, std::list<std::shared_ptr<Connection>>()))
            .second) {
      emit portAdded(pType, pIndex);
      return true;
    } else {
      GET_INFO();
    }
    break;
  default:
    break;
  }

  return false;
}

bool Node::removePort(PortType pType, PortIndex pIndex) {
  switch (pType) {
  case PortType::In:
    if (_inConnections.erase(pIndex)) {
      emit portRemoved(pType, pIndex);
      return true;
    }
    break;
  case PortType::Out:
    if (_outConnections.erase(pIndex)) {
      emit portRemoved(pType, pIndex);
      return true;
    }
    break;
  default:
    break;
  }
  return false;
}

QJsonObject Node::save() const {
  QJsonObject nodeJson;

  nodeJson["id"] = id().toString();

  nodeJson["model"] = _nodeDataModel->save();

  // only out connections
  QJsonArray connections;
  for (const auto &i : _outConnections) {
    for (const auto &j : i.second) {
      connections.push_back(j->save());
    }
  }

  nodeJson["connections"] = connections;

  return nodeJson;
}

QUuid Node::id() const {
  return _uid;
}

QtNodes::NodeDataModel *Node::nodeDataModel() const {
  return _nodeDataModel.get();
}

std::list<std::shared_ptr<Connection>> const &
Node::connections(PortType pType, PortIndex idx) const {
  CHECK_OUT_OF_RANGE(return pType == PortType::In ? _inConnections.at(idx)
                                                  : _outConnections.at(idx));
}

std::list<std::shared_ptr<Connection>> &Node::connections(PortType  pType,
                                                          PortIndex idx) {
  CHECK_OUT_OF_RANGE(return pType == PortType::In ? _inConnections.at(idx)
                                                  : _outConnections.at(idx));
}

void Node::propagateData(std::shared_ptr<NodeData> nodeData,
                         PortIndex                 inPortIndex) const {
  _nodeDataModel->setInData(std::move(nodeData), inPortIndex);
}

void Node::onDataUpdated(PortIndex index) {
  auto  nodeData = _nodeDataModel->outData(index);
  auto &conns    = connections(PortType::Out, index);

  for (auto const &c : conns) {
    c->propagateData(nodeData);
  }
}

void Node::restore(const QJsonObject &obj) {
  _nodeDataModel->restore(obj["model"].toObject());
}

unsigned Node::nodePortCount(PortType type) {
  return _nodeDataModel->nPorts(type);
}

std::list<QtNodes::PortIndex> Node::nodePortIndexes(PortType type) {
  return _nodeDataModel->ports(type);
}

bool Node::nodePortCaptionVisibility(PortType type, PortIndex index) const {
  return _nodeDataModel->portCaptionVisibility(type, index);
}
