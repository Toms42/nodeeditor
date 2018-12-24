// DataFlowModel.cpp

#include "DataFlowModel.hpp"
#include "Connection.hpp"
#include "DataModelRegistry.hpp"
#include "Node.hpp"
#include "checker.hpp"

namespace QtNodes {

DataFlowModel::DataFlowModel(std::shared_ptr<DataModelRegistry> registry)
    : _registry(std::move(registry)) {}

// FlowSceneModel read interface
QStringList DataFlowModel::modelRegistry() const {
  QStringList list;
  for (const auto &item : _registry->registeredModelCreators()) {
    list << item.first;
  }
  return list;
}

QString DataFlowModel::nodeTypeCategory(QString const &name) const {
  auto iter = _registry->registeredModelsCategoryAssociation().find(name);

  if (iter != _registry->registeredModelsCategoryAssociation().end()) {
    return iter->second;
  }
  return {};
}

bool DataFlowModel::getTypeConvertable(TypeConverterId const &id) const {
  return static_cast<bool>(_registry->getTypeConverter(id.first, id.second));
}

QList<QUuid> DataFlowModel::nodeUUids() const {
  QList<QUuid> ret;

  // extract the keys
  std::transform(_nodes.begin(),
                 _nodes.end(),
                 std::back_inserter(ret),
                 [](const auto &pair) { return pair.first; });

  return ret;
}

NodeIndex DataFlowModel::nodeIndex(const QUuid &ID) const {
  auto iter = _nodes.find(ID);
  if (iter == _nodes.end())
    return {};

  return createIndex(ID, iter->second.get());
}
QString DataFlowModel::nodeTypeIdentifier(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());

  return node->nodeImp()->name();
}
QString DataFlowModel::nodeCaption(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  // auto *node = static_cast<Node *>(index.internalPointer());

  return {};
  // if (!node->nodeImp()->captionVisible())
  //  return {};

  // return node->nodeDataModel()->caption();
}

QPointF DataFlowModel::nodeLocation(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());

  return node->position();
}
QWidget *DataFlowModel::nodeWidget(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());

  return node->nodeImp()->embeddedWidget();
}
bool DataFlowModel::nodeResizable(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());

  return node->nodeImp()->resizable();
}
NodeValidationState
DataFlowModel::nodeValidationState(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());

  return node->nodeImp()->validationState();
}

QString DataFlowModel::nodeValidationMessage(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());

  return node->nodeImp()->validationMessage();
}

NodePainterDelegate *
DataFlowModel::nodePainterDelegate(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());

  return node->nodeImp()->painterDelegate();
}

bool DataFlowModel::addPort(const NodeIndex &nIndex,
                            PortType         pType,
                            PortIndex        pIndex) {
  Q_ASSERT(nIndex.isValid());
  auto *node = reinterpret_cast<Node *>(nIndex.internalPointer());

  bool retval = node->addPort(pType, pIndex);
  emit updateNode(nIndex);
  return retval;
}

unsigned int DataFlowModel::nodePortCount(NodeIndex const &index,
                                          PortType         portType) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  return node->nodeImp()->nPorts(portType);
}

std::list<PortIndex> DataFlowModel::nodePortIndexes(const NodeIndex &index,
                                                    PortType portType) const {
  std::list<PortIndex> retval;
  auto *               node = reinterpret_cast<Node *>(index.internalPointer());
  return node->nodeImp()->ports(portType);
  ;
}

QString DataFlowModel::nodePortCaption(NodeIndex const &index,
                                       PortIndex        pIndex,
                                       PortType         portType) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  if (node->nodeImp()->portCaptionVisibility(portType, pIndex)) {
    return node->nodeImp()->portCaption(portType, pIndex);
  }
  return "";
}

NodeDataType DataFlowModel::nodePortDataType(NodeIndex const &index,
                                             PortIndex        pIndex,
                                             PortType         portType) const {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());

  return node->nodeImp()->dataType(portType, pIndex);
}
ConnectionPolicy DataFlowModel::nodePortConnectionPolicy(
    NodeIndex const &index, PortIndex pIndex, PortType portType) const {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());

  if (portType == PortType::In) {
    return ConnectionPolicy::One;
  }
  return node->nodeImp()->portOutConnectionPolicy(pIndex);
}

std::vector<std::pair<NodeIndex, PortIndex>> DataFlowModel::nodePortConnections(
    NodeIndex const &index, PortIndex id, PortType portType) const {
  std::vector<std::pair<NodeIndex, PortIndex>> ret;
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  // construct connections
  for (const auto &conn : node->connections(portType, id)) {
    ret.emplace_back(nodeIndex(conn->getNode(oppositePort(portType))->id()),
                     conn->getPortIndex(oppositePort(portType)));
  }
  return ret;
}

bool DataFlowModel::removePort(const NodeIndex &nodeIndex,
                               PortType         portType,
                               PortIndex        portIndex) {
  bool success{false};
  for (auto &i : nodePortConnections(nodeIndex, portIndex, portType)) {
    if (portType == PortType::In) {
      success = removeConnection(i.first, i.second, nodeIndex, portIndex);
    } else {
      success = removeConnection(nodeIndex, portIndex, i.first, i.second);
    }
    if (!success) {
      return false;
    }
  }

  bool returnval =
      reinterpret_cast<QtNodes::Node *>(nodeIndex.internalPointer())
          ->removePort(portType, portIndex);
  if (returnval) {
    for (auto &i : nodePortIndexes(nodeIndex, portType)) {
      for (auto &j : nodePortConnections(nodeIndex, i, portType)) {
        if (portType == PortType::In) {
          emit updateConnection(j.first, j.second, nodeIndex, i);
        } else {
          emit updateConnection(nodeIndex, i, j.first, j.second);
        }
      }
    }

    emit updateNode(nodeIndex);
  }
  return returnval;
}

// FlowSceneModel write interface
bool DataFlowModel::removeConnection(NodeIndex const &leftNodeIdx,
                                     PortIndex        leftPortID,
                                     NodeIndex const &rightNodeIdx,
                                     PortIndex        rightPortID) {
  Q_ASSERT(leftNodeIdx.isValid());
  Q_ASSERT(rightNodeIdx.isValid());

  auto *leftNode  = reinterpret_cast<Node *>(leftNodeIdx.internalPointer());
  auto *rightNode = reinterpret_cast<Node *>(rightNodeIdx.internalPointer());

  ConnectionID connID;
  connID.lNodeID = leftNodeIdx.id();
  connID.rNodeID = rightNodeIdx.id();
  connID.lPortID = leftPortID;
  connID.rPortID = rightPortID;

  try {
    _connections.at(connID)->propagateEmptyData();
  } catch (std::out_of_range) {
    GET_INFO();
  }

  // remove it from the nodes
  auto &leftConns = leftNode->connections(PortType::Out, leftPortID);
  auto  iter =
      std::find_if(leftConns.begin(), leftConns.end(), [&](Connection *conn) {
        return conn->id() == connID;
      });
  Q_ASSERT(iter != leftConns.end());
  leftConns.erase(iter);

  auto &rightConns = rightNode->connections(PortType::In, rightPortID);
  iter             = std::find_if(rightConns.begin(),
                      rightConns.end(),
                      [&](Connection *conn) { return conn->id() == connID; });
  Q_ASSERT(iter != rightConns.end());
  rightConns.erase(iter);

  // remove it from the map
  _connections.erase(connID);

  // tell the view
  emit connectionRemoved(leftNodeIdx, leftPortID, rightNodeIdx, rightPortID);

  return true;
}

bool DataFlowModel::addConnection(NodeIndex const &leftNodeIdx,
                                  PortIndex        leftPortID,
                                  NodeIndex const &rightNodeIdx,
                                  PortIndex        rightPortID) {
  Q_ASSERT(leftNodeIdx.isValid());
  Q_ASSERT(rightNodeIdx.isValid());

  auto *leftNode  = static_cast<Node *>(leftNodeIdx.internalPointer());
  auto *rightNode = static_cast<Node *>(rightNodeIdx.internalPointer());

  // type conversions
  TypeConverter conv = {};
  auto ltype         = nodePortDataType(leftNodeIdx, leftPortID, PortType::Out);
  auto rtype = nodePortDataType(rightNodeIdx, rightPortID, PortType::In);
  if (ltype.id != rtype.id) {
    conv = _registry->getTypeConverter(ltype, rtype);
  }

  addConnection(leftNode, leftPortID, rightNode, rightPortID, conv);

  return true;
}

bool DataFlowModel::removeNode(NodeIndex const &index) {
  Q_ASSERT(index.isValid());

  // remove it from the map
  _nodes.erase(index.id());

  // tell the view
  emit nodeRemoved(index.id());

  return true;
}
QUuid DataFlowModel::addNode(const QString &typeID, QPointF const &location) {
  auto nodeid = QUuid::createUuid();
  addNode(typeID, location, nodeid);
  return nodeid;
}

QUuid DataFlowModel::addNode(QString const &typeID,
                             QPointF const &location,
                             QUuid const &  nodeid) {
  // create the NodeDataModel
  auto model = _registry->create(typeID);
  if (!model) {
    return {};
  }
  return addNode(std::move(model), location, nodeid);
}

QUuid DataFlowModel::addNode(std::unique_ptr<NodeImp> &&model,
                             QPointF const &            location,
                             QUuid const &              nodeid) {
  connect(model.get(), &NodeImp::dataUpdated, this, [this, nodeid](PortIndex) {
    emit nodeValidationUpdated(nodeIndex(nodeid));
  });

  connect(model.get(),
          &NodeImp::portRemoved,
          this,
          [this, nodeid](PortType pType, PortIndex pIndex) {
            this->removePort(nodeIndex(nodeid), pType, pIndex);
          });

  connect(model.get(),
          &NodeImp::portAdded,
          this,
          [this, nodeid](PortType pType, PortIndex pIndex) {
            this->addPort(nodeIndex(nodeid), pType, pIndex);
          });

  // create a node
  auto node = std::make_unique<Node>(std::move(model), nodeid);

  node->setPosition(location);

  // cache the pointer so the connection can be made
  auto nodePtr = node.get();

  // add it to the map
  if (!_nodes.insert(std::make_pair(nodeid, std::move(node))).second) {
    GET_INFO();
  }

  // connect to the geometry gets updated
  connect(
      nodePtr, &Node::positionChanged, this, [this, nodeid](QPointF const &) {
        emit nodeMoved(nodeIndex(nodeid));
      });

  // tell the view
  emit nodeAdded(nodeid);

  return nodeid;
}

ConnectionID DataFlowModel::addConnection(Node *        leftNode,
                                          PortIndex     leftPortID,
                                          Node *        rightNode,
                                          PortIndex     rightPortID,
                                          TypeConverter conv) {
  ConnectionID connID;
  connID.lNodeID = leftNode->id();
  connID.rNodeID = rightNode->id();
  connID.lPortID = leftPortID;
  connID.rPortID = rightPortID;

  // create the connection
  auto conn = std::make_shared<Connection>(
      *rightNode, rightPortID, *leftNode, leftPortID, conv);
  if (!_connections.insert(std::make_pair(connID, conn)).second) {
    GET_INFO();
  }

  // add it to the nodes
  leftNode->connections(PortType::Out, leftPortID).push_back(conn.get());
  rightNode->connections(PortType::In, rightPortID).push_back(conn.get());

  // process data
  conn->getNode(PortType::Out)
      ->onDataUpdated(conn->getPortIndex(PortType::Out));

  // tell the view the connection was added
  emit connectionAdded(nodeIndex(leftNode->id()),
                       leftPortID,
                       nodeIndex(rightNode->id()),
                       rightPortID);

  return connID;
}

bool DataFlowModel::moveNode(NodeIndex const &index, QPointF newLocation) {
  Q_ASSERT(index.isValid());

  auto *node = static_cast<Node *>(index.internalPointer());
  node->setPosition(newLocation);

  // no need to emit, it's done by the function already
  return true;
}

std::unordered_map<ConnectionID, QtNodes::DataFlowModel::SharedConnection> &
DataFlowModel::connections() {
  return _connections;
}

std::unordered_map<QUuid, QtNodes::DataFlowModel::UniqueNode> &
DataFlowModel::nodes() {
  return _nodes;
}

DataModelRegistry &DataFlowModel::registry() {
  return *_registry;
}

void DataFlowModel::setRegistry(std::shared_ptr<DataModelRegistry> registry) {
  _registry = std::move(registry);
}

} // namespace QtNodes
