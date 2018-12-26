#include "Node.hpp"
#include "Connection.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "FlowScene.hpp"
#include "NodeDataModel.hpp"
#include "NodeGraphicsObject.hpp"
#include "checker.hpp"
#include <QtCore/QObject>
#include <utility>

using QtNodes::Connection;
using QtNodes::Node;

Node::Node(std::unique_ptr<NodeDataModel> &&nodeImp, QUuid const &id)
    : _uid(id)
    , nodeImp_(std::move(nodeImp)) {
  // propagate data: model => node
  connect(
      nodeImp_.get(), &NodeDataModel::dataUpdated, this, &Node::onDataUpdated);

  for (const auto &i : nodeImp_->inPorts_) {
    _inConnections.insert(std::pair(i.first, std::vector<Connection *>()));
  }
  for (const auto &i : nodeImp_->outPorts_) {
    _outConnections.insert(std::pair(i.first, std::vector<Connection *>()));
  }
}

Node::~Node() = default;

bool Node::addPort(PortType pType, PortIndex pIndex) {
  switch (pType) {
  case PortType::In:
    if (_inConnections.insert(std::pair(pIndex, std::vector<Connection *>()))
            .second) {
      emit portAdded(pType, pIndex);
      return true;
    } else {
      GET_INFO();
    }
    break;
  case PortType::Out:
    if (_outConnections.insert(std::pair(pIndex, std::vector<Connection *>()))
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

  nodeJson["model"] = nodeImp_->save();

  QJsonObject obj;
  obj["x"]             = _pos.x();
  obj["y"]             = _pos.y();
  nodeJson["position"] = obj;

  return nodeJson;
}

void Node::restore(QJsonObject const &json) {
  QJsonObject positionJson = json["position"].toObject();
  QPointF     point(positionJson["x"].toDouble(), positionJson["y"].toDouble());
  setPosition(point);

  nodeImp_->restore(json["model"].toObject());
}

QUuid Node::id() const {
  return _uid;
}

QtNodes::NodeDataModel *Node::nodeImp() const {
  return nodeImp_.get();
}

QPointF Node::position() const {
  return _pos;
}

void Node::setPosition(QPointF const &newPos) {
  _pos = newPos;

  emit positionChanged(newPos);
}

std::vector<Connection *> const &Node::connections(PortType  pType,
                                                   PortIndex idx) const {
  CHECK_OUT_OF_RANGE(return pType == PortType::In ? _inConnections.at(idx)
                                                  : _outConnections.at(idx));
}

std::vector<Connection *> &Node::connections(PortType pType, PortIndex idx) {
  CHECK_OUT_OF_RANGE(return pType == PortType::In ? _inConnections.at(idx)
                                                  : _outConnections.at(idx));
}

void Node::propagateData(std::shared_ptr<NodeData> nodeData,
                         PortIndex                 inPortIndex) const {
  nodeImp_->setInData(std::move(nodeData), inPortIndex);
}

void Node::onDataUpdated(PortIndex index) {
  auto  nodeData = nodeImp_->outData(index);
  auto &conns    = connections(PortType::Out, index);

  for (auto const &c : conns) {
    c->propagateData(nodeData);
  }
}
