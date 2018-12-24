// NodeImp.cpp

#include "NodeImp.hpp"
#include "StyleCollection.hpp"
#include "checker.hpp"

namespace QtNodes {
NodeImp::NodeImp()
    : nodeStyle_{StyleCollection::nodeStyle()} {}

NodeImp::~NodeImp() = default;

bool NodeImp::addPort(PortType type, PortIndex index, Port port) {
  switch (type) {
  case PortType::In:
    if (inPorts_.insert(std::pair(index, port)).second) {
      emit portAdded(type, index);
      return true;
    }
    break;
  case PortType::Out:
    if (outPorts_.insert(std::pair(index, port)).second) {
      emit portAdded(type, index);
      return true;
    }
    break;
  default:
    break;
  }
  return false;
}

bool NodeImp::removePort(PortType type, PortIndex index) {
  switch (type) {
  case PortType::In:
    if (inPorts_.erase(index)) {
      emit portRemoved(type, index);
      return true;
    }
    break;
  case PortType::Out:
    if (outPorts_.erase(index)) {
      emit portRemoved(type, index);
      return true;
    }
    break;
  default:
    break;
  }
  return false;
}

const NodeStyle &NodeImp::nodeStyle() const {
  return nodeStyle_;
}

void NodeImp::setNodeStyle(const NodeStyle &style) {
  nodeStyle_ = style;
}

void NodeImp::setInData(std::shared_ptr<NodeData> data, PortIndex index) {
  if (auto found = inPorts_.find(index); found != inPorts_.end()) {
    found->second.handle(data);
  }
}

std::shared_ptr<NodeData> NodeImp::outData(PortIndex index) {
  if (auto found = outPorts_.find(index); found != outPorts_.end()) {
    return found->second.handle(nullptr);
  }
  return nullptr;
}

bool NodeImp::resizable() const {
  return true;
}

NodeValidationState NodeImp::validationState() const {
  return NodeValidationState::Valid;
}

QString NodeImp::validationMessage() const {
  return "";
}

QJsonObject NodeImp::save() const {
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}

NodeDataType NodeImp::dataType(PortType portType, PortIndex portIndex) const {
  switch (portType) {
  case PortType::In:
    if (auto found = inPorts_.find(portIndex); found != inPorts_.end()) {
      return found->second.dataType;
    }
    break;
  case PortType::Out:
    if (auto found = outPorts_.find(portIndex); found != outPorts_.end()) {
      return found->second.dataType;
    }
    break;
  default:
    break;
  }
  return NodeDataType();
}

unsigned int NodeImp::nPorts(PortType type) const {
  switch (type) {
  case PortType::In:
    return inPorts_.size();
    break;
  case PortType::Out:
    return outPorts_.size();
    break;
  default:
    break;
  }
  return 0;
}

std::list<PortIndex> NodeImp::ports(PortType type) const {
  std::list<PortIndex> retval;

  const std::map<PortIndex, Port> *port_map{};

  switch (type) {
  case PortType::In:
    port_map = &inPorts_;
    break;
  case PortType::Out:
    port_map = &outPorts_;
    break;
  default:
    return retval;
    break;
  }

  for (auto &i : *port_map) {
    retval.push_back(i.first);
  }

  return retval;
}

QString NodeImp::portCaption(PortType type, PortIndex index) const {
  switch (type) {
  case PortType::In:
    if (auto found = inPorts_.find(index); found != inPorts_.end()) {
      return found->second.caption;
    }
    break;
  case PortType::Out:
    if (auto found = outPorts_.find(index); found != outPorts_.end()) {
      return found->second.caption;
    }
    break;
  default:
    break;
  }
  return "";
}

bool NodeImp::portCaptionVisibility(PortType type, PortIndex index) const {
  switch (type) {
  case PortType::In:
    if (auto found = inPorts_.find(index); found != inPorts_.end()) {
      return found->second.captionVisible;
    }
    break;
  case PortType::Out:
    if (auto found = outPorts_.find(index); found != outPorts_.end()) {
      return found->second.captionVisible;
    }
    break;
  default:
    break;
  }
  return false;
}

ConnectionPolicy NodeImp::portOutConnectionPolicy(PortIndex index) const {
  try {
    return outPorts_.at(index).policy;
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

NodePainterDelegate *NodeImp::painterDelegate() const {
  return nullptr;
}

} // namespace QtNodes
