#include "DataFlowScene.hpp"
#include "Connection.hpp"
#include "DataFlowModel.hpp"
#include "DataModelRegistry.hpp"
#include "Node.hpp"
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>

#include "checker.hpp"

using QtNodes::Connection;
using QtNodes::ConnectionID;
using QtNodes::ConnectionPolicy;
using QtNodes::DataFlowModel;
using QtNodes::DataFlowScene;
using QtNodes::DataModelRegistry;
using QtNodes::Node;
using QtNodes::NodeDataType;
using QtNodes::NodeIndex;
using QtNodes::NodePainterDelegate;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;

DataFlowScene::DataFlowScene(std::shared_ptr<DataModelRegistry> registry,
                             QObject *parent)
    : FlowScene(new DataFlowModel(std::move(registry)), parent) {
  _dataFlowModel = static_cast<DataFlowModel *>(model());
}

DataFlowScene::~DataFlowScene() { delete _dataFlowModel; }

std::shared_ptr<Connection>
DataFlowScene::createConnection(Node &nodeIn, PortIndex portIndexIn,
                                Node &nodeOut, PortIndex portIndexOut,
                                TypeConverter const &converter) {
  auto connid = _dataFlowModel->addConnection(&nodeOut, portIndexOut, &nodeIn,
                                              portIndexIn, converter);

  try {
    return _dataFlowModel->connections().at(connid);
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

std::shared_ptr<Connection>
DataFlowScene::restoreConnection(QJsonObject const &connectionJson) {
  QUuid nodeInId = QUuid(connectionJson["in_id"].toString());
  QUuid nodeOutId = QUuid(connectionJson["out_id"].toString());

  PortIndex portIndexIn = connectionJson["in_index"].toInt();
  PortIndex portIndexOut = connectionJson["out_index"].toInt();

  ConnectionID connId;
  connId.lNodeID = nodeOutId;
  connId.rNodeID = nodeInId;

  connId.lPortID = portIndexOut;
  connId.rPortID = portIndexIn;

  if (!_dataFlowModel->addConnection(
          _dataFlowModel->nodeIndex(nodeOutId), connId.lPortID,
          _dataFlowModel->nodeIndex(nodeInId), connId.rPortID))
    return nullptr;

  try {
    return _dataFlowModel->connections().at(connId);
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

void DataFlowScene::deleteConnection(Connection &connection) {
  auto deleted = _dataFlowModel->removeConnection(
      _dataFlowModel->nodeIndex(connection.getNode(PortType::Out)->id()),
      connection.getPortIndex(PortType::Out),
      _dataFlowModel->nodeIndex(connection.getNode(PortType::In)->id()),
      connection.getPortIndex(PortType::Out));
  Q_ASSERT(deleted);
}

Node &DataFlowScene::createNode(std::unique_ptr<NodeImp> &&dataModel) {
  auto uid = _dataFlowModel->addNode(std::move(dataModel), {0.0, 0.0});

  try {
    return *_dataFlowModel->nodes().at(uid);
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

Node &DataFlowScene::restoreNode(QJsonObject const &nodeJson) {
  QString modelName = nodeJson["model"].toObject()["name"].toString();

  auto uid = QUuid(nodeJson["id"].toString());

  // set initial point to 0, 0, will be updated in Node::restore
  _dataFlowModel->addNode(modelName, QPointF(0, 0), uid);

  try {
    auto &node = *_dataFlowModel->nodes().at(uid);
    node.restore(nodeJson);

    return node;
  } catch (std::out_of_range) {
    GET_INFO();
  }
}

void DataFlowScene::removeNode(Node &node) {
  model()->removeNodeWithConnections(model()->nodeIndex(node.id()));
}

DataModelRegistry &DataFlowScene::registry() const {
  return _dataFlowModel->registry();
}

void DataFlowScene::setRegistry(std::shared_ptr<DataModelRegistry> registry) {
  _dataFlowModel->setRegistry(registry);
}

void DataFlowScene::iterateOverNodes(
    std::function<void(Node *)> const &visitor) {
  for (auto const &node : _dataFlowModel->nodes()) {
    visitor(node.second.get());
  }
}

void DataFlowScene::iterateOverNodeData(
    std::function<void(NodeImp *)> const &visitor) {
  for (auto const &node : _dataFlowModel->nodes()) {
    visitor(node.second->nodeImp());
  }
}

void DataFlowScene::iterateOverNodeDataDependentOrder(
    std::function<void(NodeImp *)> const &visitor) {
  std::set<QUuid> visitedNodesSet;

  // A leaf node is a node with no input ports, or all possible input ports
  // empty
  auto isNodeLeaf = [](Node const &node, NodeImp const &model) {
    for (auto &i : model.ports(PortType::In)) {
      auto connections = node.connections(PortType::In, i);
      if (!connections.empty()) {
        return false;
      }
    }

    return true;
  };

  // Iterate over "leaf" nodes
  for (auto const &_node : _dataFlowModel->nodes()) {
    auto const &node = _node.second;
    auto model = node->nodeImp();

    if (isNodeLeaf(*node, *model)) {
      visitor(model);
      visitedNodesSet.insert(node->id());
    }
  }

  auto areNodeInputsVisitedBefore = [&](Node const &node,
                                        NodeImp const &model) {
    for (auto &i : model.ports(PortType::In)) {
      auto connections = node.connections(PortType::In, i);

      for (auto &conn : connections) {
        if (visitedNodesSet.find(conn->getNode(PortType::Out)->id()) ==
            visitedNodesSet.end()) {
          return false;
        }
      }
    }

    return true;
  };

  // Iterate over dependent nodes
  while (_dataFlowModel->nodes().size() != visitedNodesSet.size()) {
    for (auto const &_node : _dataFlowModel->nodes()) {
      auto const &node = _node.second;
      if (visitedNodesSet.find(node->id()) != visitedNodesSet.end())
        continue;

      auto model = node->nodeImp();

      if (areNodeInputsVisitedBefore(*node, *model)) {
        visitor(model);
        visitedNodesSet.insert(node->id());
      }
    }
  }
}

QPointF DataFlowScene::getNodePosition(Node const &node) const {
  return _dataFlowModel->nodeLocation(_dataFlowModel->nodeIndex(node.id()));
}

void DataFlowScene::setNodePosition(Node &node, QPointF const &pos) const {
  _dataFlowModel->moveNode(_dataFlowModel->nodeIndex(node.id()), pos);
}

std::unordered_map<QUuid, std::unique_ptr<Node>> const &
DataFlowScene::nodes() const {
  return _dataFlowModel->nodes();
}

std::unordered_map<ConnectionID, std::shared_ptr<Connection>> const &
DataFlowScene::connections() const {
  return _dataFlowModel->connections();
}

std::vector<Node *> DataFlowScene::selectedNodes() const {
  auto ids = FlowScene::selectedNodes();
  std::vector<Node *> ret;
  ret.reserve(ids.size());

  std::transform(ids.begin(), ids.end(), std::back_inserter(ret),
                 [this](NodeIndex const &id) {
                   return _dataFlowModel->nodes().find(id.id())->second.get();
                 });

  return ret;
}

void DataFlowScene::clearScene() {
  // delete all the nodes
  while (!_dataFlowModel->nodes().empty()) {
    removeNode(*_dataFlowModel->nodes().begin()->second);
  }
}

void DataFlowScene::save() const {
  QString fileName = QFileDialog::getSaveFileName(
      nullptr, tr("Open Flow Scene"), QDir::homePath(),
      tr("Flow Scene Files (*.flow)"));

  if (!fileName.isEmpty()) {
    if (!fileName.endsWith("flow", Qt::CaseInsensitive))
      fileName += ".flow";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
      file.write(saveToMemory());
    }
  }
}

void DataFlowScene::load() {
  clearScene();

  //-------------

  QString fileName = QFileDialog::getOpenFileName(
      nullptr, tr("Open Flow Scene"), QDir::homePath(),
      tr("Flow Scene Files (*.flow)"));

  if (!QFileInfo::exists(fileName))
    return;

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly))
    return;

  QByteArray wholeFile = file.readAll();

  loadFromMemory(wholeFile);
}

QByteArray DataFlowScene::saveToMemory() const {
  QJsonObject sceneJson;

  QJsonArray nodesJsonArray;

  for (auto const &pair : _dataFlowModel->nodes()) {
    auto const &node = pair.second;

    nodesJsonArray.append(node->save());
  }

  sceneJson["nodes"] = nodesJsonArray;

  QJsonArray connectionJsonArray;
  for (auto const &pair : _dataFlowModel->connections()) {
    auto const &connection = pair.second;

    QJsonObject connectionJson = connection->save();

    if (!connectionJson.isEmpty())
      connectionJsonArray.append(connectionJson);
  }

  sceneJson["connections"] = connectionJsonArray;

  QJsonDocument document(sceneJson);

  return document.toJson();
}

void DataFlowScene::loadFromMemory(const QByteArray &data) {
  QJsonObject const jsonDocument = QJsonDocument::fromJson(data).object();

  QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

  for (int i = 0; i < nodesJsonArray.size(); ++i) {
    restoreNode(nodesJsonArray[i].toObject());
  }

  QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();

  for (int i = 0; i < connectionJsonArray.size(); ++i) {
    restoreConnection(connectionJsonArray[i].toObject());
  }
}

