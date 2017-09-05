#include "FlowScene.hpp"
#include "NodeIndex.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "NodeGraphicsObject.hpp"

namespace QtNodes {

FlowScene::FlowScene(FlowSceneModel* model) 
  : _model(model)
{
  Q_ASSERT(model != nullptr);

  connect(model, &FlowSceneModel::nodeRemoved, this, &FlowScene::nodeRemoved);
  connect(model, &FlowSceneModel::nodeAdded, this, &FlowScene::nodeAdded);
  connect(model, &FlowSceneModel::nodePortUpdated, this, &FlowScene::nodePortUpdated);
  connect(model, &FlowSceneModel::nodeValidationUpdated, this, &FlowScene::nodeValidationUpdated);
  connect(model, &FlowSceneModel::connectionRemoved, this, &FlowScene::connectionRemoved);
  connect(model, &FlowSceneModel::connectionAdded, this, &FlowScene::connectionAdded);
  connect(model, &FlowSceneModel::nodeMoved, this, &FlowScene::nodeMoved);

  // emit node added on all the existing nodes
  for (const auto& n : model->nodeUUids()) {
    nodeAdded(n);
  }
  
  // add connections
  for (const auto& n : model->nodeUUids()) {
    auto id = model->nodeIndex(n);
    Q_ASSERT(id.isValid());
    
    // query the number of ports   
    auto numPorts = model->nodePortCount(id, PortType::Out);
    
    // go through them and add the connections
    for (auto portID = 0u; portID < numPorts; ++portID) {
      // go through connections
      auto connections = model->nodePortConnections(id, portID, PortType::Out);
      
      // validate the sanity of the model--make sure if it is marked as one connection per port then there is no more than one connection
      Q_ASSERT(model->nodePortConnectionPolicy(id, portID, PortType::Out) == ConnectionPolicy::Many || connections.size() <= 1);
      
      for (const auto& conn : connections) {
        connectionAdded(id, portID, conn.first, conn.second);
      }
    }
  }
}

FlowScene::~FlowScene() = default;

NodeGraphicsObject*
FlowScene::
nodeGraphicsObject(const NodeIndex& index)
{
  auto iter = _nodeGraphicsObjects.find(index.id());
  if (iter == _nodeGraphicsObjects.end()) {
    return nullptr;
  }
  return iter->second.get();
}

std::vector<NodeIndex>
FlowScene::
selectedNodes() const {
  QList<QGraphicsItem*> graphicsItems = selectedItems();

  std::vector<NodeIndex> ret;
  ret.reserve(graphicsItems.size());

  for (QGraphicsItem* item : graphicsItems)
  {
    auto ngo = qgraphicsitem_cast<NodeGraphicsObject*>(item);

    if (ngo != nullptr)
    {
      ret.push_back(ngo->index());
    }
  }

  return ret;

}


void 
FlowScene::
nodeRemoved(const QUuid& id)
{
  this->removeItem(_nodeGraphicsObjects[id].get());
  
  // just delete it
  auto erased = _nodeGraphicsObjects.erase(id);
  Q_ASSERT(erased == 1);
}
void
FlowScene::
nodeAdded(const QUuid& newID)
{
  Q_ASSERT(!newID.isNull());

  auto index = model()->nodeIndex(newID);
  Q_ASSERT(index.isValid());

  auto ngo = std::make_unique<NodeGraphicsObject>(*this, index);
  Q_ASSERT(ngo->scene() == this);

  _nodeGraphicsObjects[index.id()] = std::move(ngo);

  nodeMoved(index);
}
void
FlowScene::
nodePortUpdated(NodeIndex const& id)
{
  
  auto thisNodeNGO = nodeGraphicsObject(id);
  Q_ASSERT(thisNodeNGO);

}
void
FlowScene::
nodeValidationUpdated(NodeIndex const& id)
{

}
void
FlowScene::
connectionRemoved(NodeIndex const& leftNode, PortIndex leftPortID, NodeIndex const& rightNode, PortIndex rightPortID)
{
  // check the model's sanity
#ifndef NDEBUG
  for (const auto& conn : model()->nodePortConnections(leftNode, leftPortID, PortType::Out)) {
    // if you fail here, then you're emitting connectionRemoved on a connection that is in the model
    Q_ASSERT (conn.first != rightNode && conn.second != rightPortID);
  }
  for (const auto& conn : model()->nodePortConnections(rightNode, rightPortID, PortType::In)) {
    // if you fail here, then you're emitting connectionRemoved on a connection that is in the model
    Q_ASSERT (conn.first != leftNode && conn.second != leftPortID);
  }
#endif

  // create a connection ID
  ConnectionID id;
  id.lNodeID = leftNode.id();
  id.rNodeID = rightNode.id();
  id.lPortID = leftPortID;
  id.rPortID = rightPortID;
  
  // cgo
  auto& cgo = *_connGraphicsObjects[id];
  
  // remove it from the nodes
  auto& lngo = *nodeGraphicsObject(leftNode);
  lngo.nodeState().eraseConnection(PortType::Out, leftPortID, cgo);
  
  auto& rngo = *nodeGraphicsObject(rightNode);
  rngo.nodeState().eraseConnection(PortType::In, rightPortID, cgo);
  
  // remove the ConnectionGraphicsObject
  _connGraphicsObjects.erase(id);
}
void
FlowScene::
connectionAdded(NodeIndex const& leftNode, PortIndex leftPortID, NodeIndex const& rightNode, PortIndex rightPortID)
{
  // check the model's sanity
#ifndef NDEBUG
  // if you fail here, then you're emitting connectionAdded on a portID that doesn't exist
  Q_ASSERT(leftPortID < model()->nodePortCount(leftNode, PortType::Out));
  Q_ASSERT(rightPortID < model()->nodePortCount(rightNode, PortType::In));

  bool checkedOut = false;
  for (const auto& conn : model()->nodePortConnections(leftNode, leftPortID, PortType::Out)) {
    if (conn.first == rightNode && conn.second == rightPortID) {
      checkedOut = true;
      break;
    }
  }
  // if you fail here, then you're emitting connectionAdded on a connection that isn't in the model
  Q_ASSERT(checkedOut);
  checkedOut = false;
  for (const auto& conn : model()->nodePortConnections(rightNode, rightPortID, PortType::In)) {
    if (conn.first == leftNode && conn.second == leftPortID) {
      checkedOut = true;
      break;
    }
  }
  // if you fail here, then you're emitting connectionAdded on a connection that isn't in the model
  Q_ASSERT(checkedOut);
#endif
  
  // create the cgo
  auto cgo = std::make_unique<ConnectionGraphicsObject>(leftNode, leftPortID, rightNode, rightPortID, *this);
  
  // add it to the nodes
  auto lngo = nodeGraphicsObject(leftNode);
  lngo->nodeState().setConnection(PortType::Out, leftPortID, *cgo);
  
  auto rngo = nodeGraphicsObject(rightNode);
  rngo->nodeState().setConnection(PortType::In, rightPortID, *cgo);
  
  // add the cgo to the map
  _connGraphicsObjects[cgo->id()] = std::move(cgo);
  
}


void
FlowScene::
nodeMoved(NodeIndex const& index) {
  _nodeGraphicsObjects[index.id()]->setPos(model()->nodeLocation(index));
}

NodeGraphicsObject*
locateNodeAt(QPointF scenePoint, FlowScene &scene,
             QTransform viewTransform)
{
  // items under cursor
  QList<QGraphicsItem*> items =
    scene.items(scenePoint,
                Qt::IntersectsItemShape,
                Qt::DescendingOrder,
                viewTransform);

  //// items convertable to NodeGraphicsObject
  std::vector<QGraphicsItem*> filteredItems;

  std::copy_if(items.begin(),
               items.end(),
               std::back_inserter(filteredItems),
               [] (QGraphicsItem * item)
    {
      return (dynamic_cast<NodeGraphicsObject*>(item) != nullptr);
    });

  if (!filteredItems.empty())
  {
    QGraphicsItem* graphicsItem = filteredItems.front();
    auto ngo = dynamic_cast<NodeGraphicsObject*>(graphicsItem);

    return ngo;
  }

  return nullptr;
}

} // namespace QtNodes
