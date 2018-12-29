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

DataFlowScene::DataFlowScene(DataFlowModel *model, QObject *parent)
    : FlowScene(model, parent) {
  _dataFlowModel = model;
}

DataFlowScene::~DataFlowScene() {}

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
  // QJsonObject sceneJson;
  // QJsonArray posJsonArray;
  return _dataFlowModel->saveToMemory();
}

void DataFlowScene::loadFromMemory(const QByteArray &data) {
  _dataFlowModel->loadFromMemory(data);
}
