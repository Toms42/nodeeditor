#include "FlowSceneModel.hpp"
#include "Node.hpp"
#include "NodeIndex.hpp"
#include <iostream>

namespace QtNodes {

FlowSceneModel::FlowSceneModel(QObject *parent)
    : QObject{parent} {}

bool FlowSceneModel::removeNodeWithConnections(QUuid uuid) {
  // delete the conenctions that node has first
  auto index             = nodeIndex(uuid);
  auto deleteConnections = [&](PortType ty) -> bool {
    for (auto &portID : nodePortIndexes(index, ty)) {
      for (const auto &conn : nodePortConnections(index, portID, ty)) {
        // try to remove it
        bool success;
        if (ty == PortType::In) {
          success = removeConnection(conn.first, conn.second, index, portID);
        } else {
          success = removeConnection(index, portID, conn.first, conn.second);
        }

        // failed, abort the node deletion
        if (!success) {
          return false;
        }
      }
    }
    return true;
  };

  bool success = deleteConnections(PortType::In);

  if (!success) {
    std::cerr << "problems\n";
    return false;
  }

  success = deleteConnections(PortType::Out);
  if (!success) {
    std::cerr << "problems\n";
    return false;
  }

  // if we get here, then try to remove the node itsself
  return removeNode(uuid);
}

NodeIndex FlowSceneModel::createIndex(const QUuid &id,
                                      void *       internalPointer) const {
  return NodeIndex(id, internalPointer, this);
}

NodeIndex FlowSceneModel::createNotValidIndex(const QUuid &id) const {
  return NodeIndex(id, nullptr, nullptr);
}

} // namespace QtNodes
