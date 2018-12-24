#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <nodes/DataModelRegistry.hpp>
#include <nodes/NodeImp.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeImp;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageShowModel : public NodeImp {
  Q_OBJECT

public:
  ImageShowModel();

  virtual ~ImageShowModel();

public:
  QString name() const override { return QString("ImageShowModel"); }

public:
  virtual QString modelName() const { return QString("Resulting Image"); }

  QWidget *embeddedWidget() override { return _label; }

  bool resizable() const override { return true; }

protected:
  bool eventFilter(QObject *object, QEvent *event) override;

private:
  QLabel *_label;

  std::shared_ptr<NodeData> _nodeData;
};
