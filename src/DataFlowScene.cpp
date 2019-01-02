#include "DataFlowScene.hpp"
#include "Connection.hpp"
#include "DataFlowModel.hpp"
#include "DataModelRegistry.hpp"
#include "Node.hpp"
#include "NodeComposite.hpp"
#include "checker.hpp"
#include <QDebug>
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
  QJsonArray jsonArr;
  for (const auto &i : items(Qt::AscendingOrder)) {
    if (auto compisite = dynamic_cast<class NodeComposite *>(i); compisite) {
      QPointF pos  = compisite->pos();
      QSize   size = compisite->size();
      QUuid   parentUuid;
      if (compisite->parentItem()) {
        if (auto parent =
                dynamic_cast<class NodeComposite *>(compisite->parentItem());
            parent) {
          parentUuid = parent->nodeIndex().id();
        }
      }

      QJsonObject jsonObj;
      if (compisite->type() == NodeComposite::Frame) {
        jsonObj["type"] = "Frame";
      } else {
        jsonObj["type"] = "Node";
      }
      jsonObj["id"]     = compisite->nodeIndex().id().toString();
      jsonObj["parent"] = parentUuid.toString();

      QJsonObject jsonPos;
      jsonPos["x"]   = pos.x();
      jsonPos["y"]   = pos.y();
      jsonObj["pos"] = jsonPos;

      QJsonObject jsonSize;
      jsonSize["width"]  = size.width();
      jsonSize["height"] = size.height();
      jsonObj["size"]    = jsonSize;

      jsonArr.push_back(jsonObj);
    }
  }
  QJsonObject jsonGeneralObject;
  jsonGeneralObject["nodes"]           = jsonArr;
  jsonGeneralObject["model_data_file"] = "model_data_file";
  jsonGeneralObject["model_datas"]     = _dataFlowModel->saveToMemory();

  return QJsonDocument{jsonGeneralObject}.toJson();
}

void DataFlowScene::loadFromMemory(const QByteArray &data) {
  for (const auto &i :
       QJsonDocument::fromJson(data).object()["nodes"].toArray()) {
    auto  jsonObj = i.toObject();
    QUuid uid{jsonObj["id"].toString()};

    auto    jsonPos = jsonObj["pos"].toObject();
    QPointF pos(jsonPos["x"].toDouble(), jsonPos["y"].toDouble());
    auto    jsonSize = jsonObj["size"].toObject();
    QSize   size(jsonSize["width"].toInt(), jsonSize["height"].toInt());
    QUuid   parent{jsonObj["parent"].toString()};

    NodeGraphicsData graphicsData{pos, size, parent};
    loadedDatas_.insert(std::pair(uid, graphicsData));
    if (jsonObj["type"] == "Frame") {
      nodeAdded(uid);
    }
  }

  auto modelObj =
      QJsonDocument::fromJson(data).object()["model_datas"].toObject();
  _dataFlowModel->loadFromMemory(QJsonDocument{modelObj}.toJson());

  // auto modelFile =
  //    QJsonDocument::fromBinaryData(data).object()["dataModelFile"].toString();
  // QFile file(modelFile);
  // if (!file.open(QIODevice::ReadOnly)) {
  //  GET_INFO();
  //}
  // QByteArray modelData = file.readAll();
  // file.close();
  //_dataFlowModel->loadFromMemory(modelData);
}

void DataFlowScene::nodeAdded(const QUuid &uuid) {
  if (!loadedDatas_.empty()) {
    if (auto found = loadedDatas_.find(uuid); found != loadedDatas_.end()) {
      FlowScene::nodeAdded(uuid);
      auto composite = nodeComposite(uuid);
      if (!composite) {
        GET_INFO();
      }
      composite->setPos(found->second.pos);
      composite->setSize(found->second.size);
      if (!found->second.parent.isNull()) {
        auto parent = nodeComposite(found->second.parent);
        if (!parent) {
          GET_INFO();
        }
        composite->setParentItem(parent);
      }

      loadedDatas_.erase(found);
    } else {
      GET_INFO();
    }
  } else {
    FlowScene::nodeAdded(uuid);
  }
}
