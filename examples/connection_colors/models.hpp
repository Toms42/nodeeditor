#pragma once

#include <QtCore/QObject>
#include <memory>
#include <nodes/NodeData.hpp>
#include <nodes/NodeDataModel.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
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

class SimpleNodeData : public NodeData {
public:
  NodeDataType type() const override {
    return NodeDataType{"SimpleData", "Simple Data"};
  }
};

//------------------------------------------------------------------------------

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NaiveDataModel : public NodeDataModel {
  Q_OBJECT

public:
  NaiveDataModel() {
    QtNodes::Port inPort;
    inPort.captionVisible = false;
    inPort.dataType       = MyNodeData{}.type();
    inPort.handle         = [](std::shared_ptr<NodeData>) { return nullptr; };

    QtNodes::Port outPort;
    outPort.captionVisible = false;
    outPort.dataType       = MyNodeData{}.type();
    outPort.handle         = [](std::shared_ptr<NodeData>) { return nullptr; };

    QtNodes::Port inPort1;
    inPort1.captionVisible = false;
    inPort1.dataType       = SimpleNodeData{}.type();
    inPort1.handle         = [](std::shared_ptr<NodeData>) { return nullptr; };

    QtNodes::Port outPort1;
    outPort1.captionVisible = false;
    outPort1.dataType       = SimpleNodeData{}.type();
    outPort1.handle         = [](std::shared_ptr<NodeData>) { return nullptr; };

    addPort(PortType::In, 1, inPort);
    addPort(PortType::In, 2, inPort1);

    addPort(PortType::Out, 1, outPort);
    addPort(PortType::Out, 2, outPort1);
  }

  virtual ~NaiveDataModel() {}

public:
  // QString caption() const override { return QString("Naive Data Model"); }

  QString name() const override { return QString("NaiveDataModel"); }

public:
  QWidget *embeddedWidget() override { return nullptr; }
};
