#include "DataFlowScene.hpp"
#include "Connection.hpp"
#include "DataFlowModel.hpp"
#include "DataModelRegistry.hpp"
#include "Node.hpp"
#include "NodeComposite.hpp"
#include "checker.hpp"
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <utility>

using QtNodes::Connection;
using QtNodes::ConnectionID;
using QtNodes::DataFlowModel;
using QtNodes::DataFlowScene;
using QtNodes::DataModelRegistry;
using QtNodes::Node;
using QtNodes::NodeIndex;
using QtNodes::PortIndex;

DataFlowScene::DataFlowScene(std::shared_ptr<DataModelRegistry> registry,
                             QObject *                          parent)
    : FlowScene(new DataFlowModel(std::move(registry)), parent) {
  _dataFlowModel = reinterpret_cast<DataFlowModel *>(model());
}

DataFlowScene::~DataFlowScene() {
  delete _dataFlowModel;
}

DataModelRegistry &DataFlowScene::registry() const {
  return _dataFlowModel->registry();
}

void DataFlowScene::setRegistry(std::shared_ptr<DataModelRegistry> registry) {
  _dataFlowModel->setRegistry(std::move(registry));
}

void DataFlowScene::clearScene() {
  // delete all the nodes
  for (auto &i : this->items()) {
    recursivelyRemove(dynamic_cast<NodeComposite *>(i));
  }
}

void DataFlowScene::save() const {
  QString fileName =
      QFileDialog::getSaveFileName(nullptr,
                                   tr("Open Flow Scene"),
                                   QDir::homePath(),
                                   tr("Flow Scene Files (*.flow)"));

  if (!fileName.isEmpty()) {
    if (!fileName.endsWith("flow", Qt::CaseInsensitive)) {
      fileName += ".flow";
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
      file.write(saveToMemory());
      file.close();
    }
  }
}

void DataFlowScene::load() {
  clearScene();

  //-------------

  QString fileName =
      QFileDialog::getOpenFileName(nullptr,
                                   tr("Open Flow Scene"),
                                   QDir::homePath(),
                                   tr("Flow Scene Files (*.flow)"));

  if (!QFileInfo::exists(fileName)) {
    return;
  }

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
    return;
  }

  QByteArray wholeFile = file.readAll();
  file.close();

  loadFromMemory(wholeFile);
}

QByteArray DataFlowScene::saveToMemory() const {
  QJsonObject sceneJson;

  // QJsonArray nodesJsonArray;

  // for (auto const &pair : _dataFlowModel->nodes()) {
  //  auto const &node = pair.second;

  //  nodesJsonArray.append(node->save());
  //}

  // sceneJson["nodes"] = nodesJsonArray;

  // QJsonArray connectionJsonArray;
  // for (auto const &pair : _dataFlowModel->connections()) {
  //  auto const &connection = pair.second;

  //  QJsonObject connectionJson = connection->save();

  //  if (!connectionJson.isEmpty()) {
  //    connectionJsonArray.append(connectionJson);
  //  }
  //}

  // sceneJson["connections"] = connectionJsonArray;

  QJsonDocument document(sceneJson);

  return document.toJson();
}

void DataFlowScene::loadFromMemory(const QByteArray &data) {
  QJsonObject const jsonDocument = QJsonDocument::fromJson(data).object();

  QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

  for (int i = 0; i < nodesJsonArray.size(); ++i) {
    //  restoreNode(nodesJsonArray[i].toObject());
  }

  QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();

  for (int i = 0; i < connectionJsonArray.size(); ++i) {
    //  restoreConnection(connectionJsonArray[i].toObject());
  }
}
