// DataFlowModel.cpp

#include "DataFlowModel.hpp"
#include "Connection.hpp"
#include "DataModelRegistry.hpp"
#include "Node.hpp"
#include "checker.hpp"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <iostream>

namespace QtNodes {

DataFlowModel::DataFlowModel(std::shared_ptr<DataModelRegistry> registry,
                             QObject *                          parent)
    : FlowSceneModel{parent}
    , _registry(std::move(registry)) {}

DataFlowModel::~DataFlowModel() {}

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
  if (iter == _nodes.end()) {
    return createNotValidIndex(ID);
  }

  return createIndex(ID, iter->second.get());
}
QString DataFlowModel::nodeTypeIdentifier(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  return node->nodeCaption();
}
QString DataFlowModel::nodeCaption(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  // auto *node = static_cast<Node *>(index.internalPointer());

  return {};
  // if (!node->nodeImp()->captionVisible())
  //  return {};

  // return node->nodeDataModel()->caption();
}

QWidget *DataFlowModel::nodeWidget(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  return node->nodeWidget();
}

NodeValidationState
DataFlowModel::nodeValidationState(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  return node->nodeValidationState();
}

QString DataFlowModel::nodeValidationMessage(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  return node->nodeValidationMessage();
}

NodePainterDelegate *
DataFlowModel::nodePainterDelegate(NodeIndex const &index) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  return node->nodePainterDelegate();
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

  return node->nodePortCount(portType);
}

std::list<PortIndex> DataFlowModel::nodePortIndexes(const NodeIndex &index,
                                                    PortType portType) const {
  std::list<PortIndex> retval;
  auto *               node = reinterpret_cast<Node *>(index.internalPointer());
  return node->nodePortIndexes(portType);
  ;
}

QString DataFlowModel::nodePortCaption(NodeIndex const &index,
                                       PortIndex        pIndex,
                                       PortType         portType) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  if (node->nodePortCaptionVisibility(portType, pIndex)) {
    return node->nodePortCaption(portType, pIndex);
  }
  return "";
}

NodeDataType DataFlowModel::nodePortDataType(NodeIndex const &index,
                                             PortIndex        pIndex,
                                             PortType         portType) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  return node->nodePortDataType(portType, pIndex);
}
ConnectionPolicy DataFlowModel::nodePortConnectionPolicy(
    NodeIndex const &index, PortIndex pIndex, PortType portType) const {
  Q_ASSERT(index.isValid());

  auto *node = reinterpret_cast<Node *>(index.internalPointer());

  if (portType == PortType::In) {
    return ConnectionPolicy::One;
  }
  return node->nodePortConnectionPolicy(PortType::Out, pIndex);
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

  // remove it from the nodes
  auto &leftConns = leftNode->connections(PortType::Out, leftPortID);
  auto  iter      = std::find_if(
      leftConns.begin(),
      leftConns.end(),
      [&](std::shared_ptr<Connection> conn) { return conn->id() == connID; });
  Q_ASSERT(iter != leftConns.end());
  (*iter)->propagateEmptyData();
  leftConns.erase(iter);

  auto &rightConns = rightNode->connections(PortType::In, rightPortID);
  iter             = std::find_if(
      rightConns.begin(),
      rightConns.end(),
      [&](std::shared_ptr<Connection> conn) { return conn->id() == connID; });
  Q_ASSERT(iter != rightConns.end());
  rightConns.erase(iter);

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

  auto *leftNode  = reinterpret_cast<Node *>(leftNodeIdx.internalPointer());
  auto *rightNode = reinterpret_cast<Node *>(rightNodeIdx.internalPointer());

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

bool DataFlowModel::removeNode(QUuid index) {
  // remove it from the map
  if (_nodes.erase(index)) {
    // tell the view
    emit nodeRemoved(index);
  }

  return true;
}
QUuid DataFlowModel::addNode(const QString &typeID) {
  auto nodeid = QUuid::createUuid();
  addNode(typeID, nodeid);
  return nodeid;
}

QUuid DataFlowModel::addNode(QString const &typeID, QUuid const &nodeid) {
  // create the NodeDataModel
  auto model = _registry->create(typeID);
  if (!model) {
    return {};
  }
  return addNode(std::move(model), nodeid);
}

QUuid DataFlowModel::addNode(std::unique_ptr<NodeDataModel> &&model,
                             QUuid const &                    nodeid) {
  connect(model.get(),
          &NodeDataModel::validationChange,
          this,
          [this, nodeid]() { emit nodeValidationUpdated(nodeIndex(nodeid)); });

  connect(model.get(),
          &NodeDataModel::portRemoved,
          this,
          [this, nodeid](PortType pType, PortIndex pIndex) {
            this->removePort(nodeIndex(nodeid), pType, pIndex);
          });

  connect(model.get(),
          &NodeDataModel::portAdded,
          this,
          [this, nodeid](PortType pType, PortIndex pIndex) {
            this->addPort(nodeIndex(nodeid), pType, pIndex);
          });

  // create a node
  auto node = std::make_unique<Node>(std::move(model), nodeid);

  // add it to the map
  if (!_nodes.insert(std::make_pair(nodeid, std::move(node))).second) {
    GET_INFO();
  }

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

  // add it to the nodes
  leftNode->connections(PortType::Out, leftPortID).push_back(conn);
  rightNode->connections(PortType::In, rightPortID).push_back(conn);

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

QJsonObject DataFlowModel::saveToMemory() const {
  QJsonObject modelJson;

  QJsonArray nodesJsonArray;

  for (auto const &pair : _nodes) {
    auto const &node = pair.second;

    nodesJsonArray.append(node->save());
  }

  modelJson["nodes"] = nodesJsonArray;

  QJsonDocument document(modelJson);

  return modelJson;
}

void DataFlowModel::loadFromMemory(const QByteArray &data) {
  QJsonObject const jsonDocument = QJsonDocument::fromJson(data).object();

  QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();
  QJsonArray connectionJsonArray;

  for (int i = 0; i < nodesJsonArray.size(); ++i) {
    restoreNode(nodesJsonArray[i].toObject());
    for (const auto &j :
         nodesJsonArray[i].toObject()["connections"].toArray()) {
      connectionJsonArray.push_back(j);
    }
  }

  for (int i = 0; i < connectionJsonArray.size(); ++i) {
    restoreConnection(connectionJsonArray[i].toObject());
  }
}

void DataFlowModel::restoreNode(const QJsonObject &nodeJson) {
  QString modelName = nodeJson["model"].toObject()["name"].toString();

  auto uid = QUuid(nodeJson["id"].toString());

  addNode(modelName, uid);

  CHECK_OUT_OF_RANGE(auto &node = *_nodes.at(uid); node.restore(nodeJson));
}

void DataFlowModel::restoreConnection(const QJsonObject &connectionJson) {
  QUuid nodeInId  = QUuid(connectionJson["in_id"].toString());
  QUuid nodeOutId = QUuid(connectionJson["out_id"].toString());

  PortIndex portIndexIn  = connectionJson["in_index"].toInt();
  PortIndex portIndexOut = connectionJson["out_index"].toInt();

  addConnection(
      nodeIndex(nodeOutId), portIndexOut, nodeIndex(nodeInId), portIndexIn);
}

} // namespace QtNodes
