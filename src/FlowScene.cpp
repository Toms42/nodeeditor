#include "FlowScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "FlowSceneModel.hpp"
#include "Node.hpp"
#include "NodeGraphicsFrame.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeImp.hpp"
#include "NodeIndex.hpp"
#include <algorithm>

#include "checker.hpp"

using QtNodes::FlowScene;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeIndex;

FlowScene::FlowScene(FlowSceneModel *model, QObject *parent)
    : QGraphicsScene(parent), _model(model) {
  Q_ASSERT(model != nullptr);

  connect(model, &FlowSceneModel::nodeRemoved, this, &FlowScene::nodeRemoved);
  connect(model, &FlowSceneModel::nodeAdded, this, &FlowScene::nodeAdded);
  connect(model, &FlowSceneModel::nodePortUpdated, this,
          &FlowScene::nodePortUpdated);
  connect(model, &FlowSceneModel::nodeValidationUpdated, this,
          &FlowScene::nodeValidationUpdated);
  connect(model, &FlowSceneModel::connectionRemoved, this,
          &FlowScene::connectionRemoved);
  connect(model, &FlowSceneModel::connectionAdded, this,
          &FlowScene::connectionAdded);
  connect(model, &FlowSceneModel::nodeMoved, this, &FlowScene::nodeMoved);
  connect(model, &FlowSceneModel::updateNode, this, &FlowScene::updateNode);
  connect(model, &FlowSceneModel::updateConnection, this,
          &FlowScene::updateConnection);

  // emit node added on all the existing nodes
  for (const auto &n : model->nodeUUids()) {
    nodeAdded(n);
  }

  // add connections
  for (const auto &n : model->nodeUUids()) {
    auto id = model->nodeIndex(n);
    Q_ASSERT(id.isValid());

    for (auto &i : model->nodePortIndexes(id, PortType::Out)) {
      auto connections = model->nodePortConnections(id, i, PortType::Out);

      // validate the sanity of the model--make sure if it is marked as one
      // connection per port then there is no more than one connection
      Q_ASSERT(model->nodePortConnectionPolicy(id, i, PortType::Out) ==
                   ConnectionPolicy::Many ||
               connections.size() <= 1);

      for (const auto &conn : connections) {
        connectionAdded(id, i, conn.first, conn.second);
      }
    }
  }

  // for some reason these end up in the wrong spot, fix that
  for (const auto &n : model->nodeUUids()) {
    auto ngo = nodeGraphicsObject(model->nodeIndex(n));
    ngo->geometry().recalculateSize();
    ngo->moveConnections();
  }
}

FlowScene::~FlowScene() = default;

NodeGraphicsObject *FlowScene::nodeGraphicsObject(const NodeIndex &index) {
  auto iter = _nodeGraphicsObjects.find(index.id());
  if (iter == _nodeGraphicsObjects.end()) {
    return nullptr;
  }
  return iter->second;
}

std::vector<NodeIndex> FlowScene::selectedNodes() const {
  QList<QGraphicsItem *> graphicsItems = selectedItems();

  std::vector<NodeIndex> ret;
  ret.reserve(graphicsItems.size());

  for (QGraphicsItem *item : graphicsItems) {
    auto ngo = qgraphicsitem_cast<NodeGraphicsObject *>(item);

    if (ngo != nullptr) {
      ret.push_back(ngo->index());
    }
  }

  return ret;
}

// model slots

void FlowScene::nodeRemoved(const QUuid &id) {
  Q_ASSERT(!id.isNull());

  try {
    auto ngo = _nodeGraphicsObjects.at(id);
#ifndef QT_NO_DEBUG
    // make sure there are no connections left
    for (const auto &connPtrSet : ngo->nodeState().getEntries(PortType::In)) {
      Q_ASSERT(connPtrSet.second.size() == 0);
    }
    for (const auto &connPtrSet : ngo->nodeState().getEntries(PortType::Out)) {
      Q_ASSERT(connPtrSet.second.size() == 0);
    }
#endif

    // just delete it
    delete ngo;
    auto erased = _nodeGraphicsObjects.erase(id);
    Q_ASSERT(erased == 1);
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

void FlowScene::nodeAdded(const QUuid &newID) {
  Q_ASSERT(!newID.isNull());

  // make sure the ID doens't exist already
  Q_ASSERT(_nodeGraphicsObjects.find(newID) == _nodeGraphicsObjects.end());

  auto index = model()->nodeIndex(newID);
  Q_ASSERT(index.isValid());

  if (reinterpret_cast<Node *>(model()->nodeIndex(newID).internalPointer())
          ->nodeImp()
          ->name() == "Frame") {
    auto ngo = new NodeGraphicsFrame(*this, index);
    Q_ASSERT(ngo->scene() == this);

    if (!_nodeGraphicsObjects.insert(std::pair(index.id(), ngo)).second) {
      GET_INFO();
    }
  } else {
    auto ngo = new NodeGraphicsObject(*this, index);
    Q_ASSERT(ngo->scene() == this);

    if (!_nodeGraphicsObjects.insert(std::pair(index.id(), ngo)).second) {
      GET_INFO();
    }
  }

  nodeMoved(index);

  // TODO (not worked) if node created in frame - frame will be set as parent
  // nodeGraphicsObject(index)->checkParent();
}

// TODO not work correctly!
void FlowScene::nodePortUpdated(NodeIndex const &id) {
  Q_ASSERT(!id.id().isNull());

  auto thisNodeNGO = nodeGraphicsObject(id);
  Q_ASSERT(thisNodeNGO != Q_NULLPTR);

  // remove all the connections
  for (auto ty : {PortType::In, PortType::Out}) {
    for (auto &i : thisNodeNGO->nodeState().getEntries(ty)) {
      try {

        while (!thisNodeNGO->nodeState().getEntries(ty).at(i.first).empty()) {

          auto conn = thisNodeNGO->nodeState().getEntries(ty).at(i.first)[0];
          // remove it from the nodes
          auto &otherNgo = *nodeGraphicsObject(conn->node(oppositePort(ty)));
          otherNgo.nodeState().eraseConnection(
              oppositePort(ty), conn->portIndex(oppositePort(ty)), *conn);

          auto &thisNgo = *nodeGraphicsObject(conn->node(ty));
          thisNgo.nodeState().eraseConnection(ty, conn->portIndex(ty), *conn);

          // remove the ConnectionGraphicsObject
          delete conn;
          auto erased = _connGraphicsObjects.erase(conn->id());
          Q_ASSERT(erased == 1);
        }
      } catch (std::out_of_range) {
        GET_INFO();
      }
    }
  }

  // recreate the NGO

  // just delete it
  delete thisNodeNGO;
  auto erased = _nodeGraphicsObjects.erase(id.id());

  Q_ASSERT(erased == 1);

  // create it
  auto ngo = new NodeGraphicsObject(*this, id);
  Q_ASSERT(ngo->scene() == this);

  if (!_nodeGraphicsObjects.insert(std::pair(id.id(), ngo)).second) {
    GET_INFO();
  }

  nodeMoved(id);

  // add the connections back
  for (auto ty : {PortType::In, PortType::Out}) {
    for (auto &i : model()->nodePortIndexes(id, ty)) {
      auto connections = model()->nodePortConnections(id, i, ty);
#ifndef QT_NO_DEBUG
      for (auto conn : connections) {
        auto remoteConns = model()->nodePortConnections(conn.first, conn.second,
                                                        oppositePort(ty));

        // if you fail here, your connections aren't self-consistent
        //      Q_ASSERT(std::any_of(remoteConns.begin(), remoteConns.end(),
        //                           [&](std::pair<NodeIndex, PortIndex>
        //                           this_conn) {
        //                             return this_conn.first == id &&
        //                                    (size_t)this_conn.second == i;
        //                           }));
      }
#endif
      // validate the sanity of the model--make sure if it is marked as one
      // connection per port then there is no more than one connection
      Q_ASSERT(model()->nodePortConnectionPolicy(id, i, ty) ==
                   ConnectionPolicy::Many ||
               connections.size() <= 1);

      for (const auto &conn : connections) {

        if (ty == PortType::Out) {
          connectionAdded(id, i, conn.first, conn.second);
        } else {
          connectionAdded(conn.first, conn.second, id, i);
        }
      }
    }
  }
}

void FlowScene::nodeValidationUpdated(NodeIndex const &id) {
  // repaint
  auto ngo = nodeGraphicsObject(id);
  ngo->geometry().recalculateSize();
  ngo->moveConnections();
  ngo->update();
}

void FlowScene::connectionRemoved(NodeIndex const &leftNode,
                                  PortIndex leftPortID,
                                  NodeIndex const &rightNode,
                                  PortIndex rightPortID) {
  // check the model's sanity
#ifndef QT_NO_DEBUG
  for (const auto &conn :
       model()->nodePortConnections(leftNode, leftPortID, PortType::Out)) {
    // if you fail here, then you're emitting connectionRemoved on a connection
    // that is in the model
    Q_ASSERT(conn.first != rightNode || conn.second != rightPortID);
  }
  for (const auto &conn :
       model()->nodePortConnections(rightNode, rightPortID, PortType::In)) {
    // if you fail here, then you're emitting connectionRemoved on a connection
    // that is in the model
    Q_ASSERT(conn.first != leftNode || conn.second != leftPortID);
  }
#endif

  // create a connection ID
  ConnectionID id;
  id.lNodeID = leftNode.id();
  id.rNodeID = rightNode.id();
  id.lPortID = leftPortID;
  id.rPortID = rightPortID;

  // cgo
  try {
    auto &cgo = *_connGraphicsObjects.at(id);
    // remove it from the nodes
    auto &lngo = *nodeGraphicsObject(leftNode);
    lngo.nodeState().eraseConnection(PortType::Out, leftPortID, cgo);

    auto &rngo = *nodeGraphicsObject(rightNode);
    rngo.nodeState().eraseConnection(PortType::In, rightPortID, cgo);

    // remove the ConnectionGraphicsObject
    delete &cgo;
    _connGraphicsObjects.erase(id);

    // update validation
    nodeValidationUpdated(leftNode);
    nodeValidationUpdated(rightNode);
  } catch (std::out_of_range) {
    GET_INFO();
  }
}
void FlowScene::connectionAdded(NodeIndex const &leftNode, PortIndex leftPortID,
                                NodeIndex const &rightNode,
                                PortIndex rightPortID) {
  // check the model's sanity
#ifndef QT_NO_DEBUG
  // Q_ASSERT(leftPortID >= 0);
  // Q_ASSERT(rightPortID >= 0);
  // if you fail here, then you're emitting connectionAdded on a portID that
  // doesn't exist
  // Q_ASSERT((size_t)leftPortID <
  //         model()->nodePortCount(leftNode, PortType::Out));
  // Q_ASSERT((size_t)rightPortID <
  //         model()->nodePortCount(rightNode, PortType::In));

  bool checkedOut = false;
  for (const auto &conn :
       model()->nodePortConnections(leftNode, leftPortID, PortType::Out)) {
    if (conn.first == rightNode && conn.second == rightPortID) {
      checkedOut = true;
      break;
    }
  }
  // if you fail here, then you're emitting connectionAdded on a connection that
  // isn't in the model
  Q_ASSERT(checkedOut);
  checkedOut = false;
  for (const auto &conn :
       model()->nodePortConnections(rightNode, rightPortID, PortType::In)) {
    if (conn.first == leftNode && conn.second == leftPortID) {
      checkedOut = true;
      break;
    }
  }
  // if you fail here, then you're emitting connectionAdded on a connection that
  // isn't in the model
  Q_ASSERT(checkedOut);
#endif

  // create the cgo
  auto cgo = new ConnectionGraphicsObject(leftNode, leftPortID, rightNode,
                                          rightPortID, *this);

  // add it to the nodes
  auto lngo = nodeGraphicsObject(leftNode);
  lngo->nodeState().setConnection(PortType::Out, leftPortID, *cgo);

  auto rngo = nodeGraphicsObject(rightNode);
  rngo->nodeState().setConnection(PortType::In, rightPortID, *cgo);

  // add the cgo to the map
  _connGraphicsObjects[cgo->id()] = cgo;
}

void FlowScene::nodeMoved(NodeIndex const &index) {
  try {
    _nodeGraphicsObjects.at(index.id())->setPos(model()->nodeLocation(index));
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

//------------------------------------------------------------------------------
namespace QtNodes {

NodeGraphicsObject *locateNodeAt(QPointF scenePoint, FlowScene &scene,
                                 QTransform const &viewTransform) {
  // items under cursor
  QList<QGraphicsItem *> items = scene.items(
      scenePoint, Qt::IntersectsItemShape, Qt::DescendingOrder, viewTransform);

  //// items convertable to NodeGraphicsObject
  std::vector<QGraphicsItem *> filteredItems;

  std::copy_if(items.begin(), items.end(), std::back_inserter(filteredItems),
               [](QGraphicsItem *item) {
                 return (dynamic_cast<NodeGraphicsObject *>(item) != nullptr);
               });

  if (!filteredItems.empty()) {
    QGraphicsItem *graphicsItem = filteredItems.front();
    auto ngo = dynamic_cast<NodeGraphicsObject *>(graphicsItem);

    return ngo;
  }

  return nullptr;
}

void FlowScene::updateNode(const NodeIndex &nodeIndex) {
  nodeGraphicsObject(nodeIndex)->update();
}

void FlowScene::updateConnection(const NodeIndex &leftNodeIndex,
                                 PortIndex leftPortIndex,
                                 const NodeIndex &rightNodeIndex,
                                 PortIndex rightPortIndex) {

  // create a connection ID
  ConnectionID id;
  id.lNodeID = leftNodeIndex.id();
  id.rNodeID = rightNodeIndex.id();
  id.lPortID = leftPortIndex;
  id.rPortID = rightPortIndex;

  // cgo
  try {
    _connGraphicsObjects.at(id)->move();
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

} // namespace QtNodes
