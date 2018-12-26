#ifndef _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_EXAMPLE2_TEXTDISPLAYDATAMODEL_HPP
#define _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_EXAMPLE2_TEXTDISPLAYDATAMODEL_HPP

#pragma once

#include "TextData.hpp"
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <iostream>
#include <nodes/NodeDataModel.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextDisplayDataModel : public NodeDataModel {
  Q_OBJECT

public:
  TextDisplayDataModel();

  ~TextDisplayDataModel() override { delete _label; }

public:
  static QString Name() { return QString("TextDisplayDataModel"); }

  QString name() const override { return TextDisplayDataModel::Name(); }

  QWidget *embeddedWidget() override { return _label; }

private:
  QLabel *_label;
};

#endif
