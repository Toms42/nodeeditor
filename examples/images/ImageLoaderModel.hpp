#pragma once

#include "PixmapData.hpp"
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
class ImageLoaderModel : public NodeDataModel {
  Q_OBJECT

public:
  ImageLoaderModel();

  ~ImageLoaderModel() override;

public:
  QString name() const override { return QString("ImageLoaderModel"); }

public:
  virtual QString modelName() const { return QString("Source Image"); }

  QWidget *embeddedWidget() override { return _label; }

protected:
  bool eventFilter(QObject *object, QEvent *event) override;

private:
  QLabel *_label;

  QPixmap _pixmap;
};
