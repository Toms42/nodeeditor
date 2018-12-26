#ifndef _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_IMAGES_IMAGESHOWMODEL_HPP
#define _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_IMAGES_IMAGESHOWMODEL_HPP

#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <iostream>
#include <nodes/DataModelRegistry.hpp>
#include <nodes/NodeDataModel.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageShowModel : public NodeDataModel {
  Q_OBJECT

public:
  ImageShowModel();

  ~ImageShowModel() override;

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

#endif
