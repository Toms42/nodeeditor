#pragma once

#include <QtCore/QObject>
#include <memory>
#include <nodes/NodeData.hpp>
#include <nodes/NodeDataModel.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class MyNodeData : public NodeData {
public:
  NodeDataType type() const override {
    return NodeDataType{"MyNodeData", "My Node Data"};
  }
};

//------------------------------------------------------------------------------

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MyDataModel : public NodeDataModel {
  Q_OBJECT

public:
  MyDataModel() {
    QtNodes::Port outPort;
    outPort.caption        = "out_caption";
    outPort.captionVisible = false;
    outPort.dataType       = MyNodeData().type();
    outPort.handle         = [this](std::shared_ptr<NodeData>) {
      Q_UNUSED(this);
      return std::make_shared<MyNodeData>();
    };

    QtNodes::Port inPort;
    inPort.caption        = "in_caption";
    inPort.captionVisible = false;
    inPort.dataType       = MyNodeData().type();
    inPort.handle         = [this](std::shared_ptr<NodeData>) {
      Q_UNUSED(this);
      return nullptr;
    };

    addPort(PortType::Out, 0, outPort);
    addPort(PortType::Out, 1, outPort);
    addPort(PortType::Out, 2, outPort);

    addPort(PortType::In, 5, inPort);
    addPort(PortType::In, 6, inPort);
    addPort(PortType::In, 7, inPort);
  }
  virtual ~MyDataModel() {}

public:
  QString name() const override { return QString("MyDataModel"); }

public:
  QJsonObject save() const override {
    QJsonObject modelJson;

    modelJson["name"] = name();

    return modelJson;
  }

public:
  QWidget *embeddedWidget() override { return nullptr; }
};
