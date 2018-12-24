#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include "TextData.hpp"

#include <nodes/NodeImp.hpp>

#include <iostream>

using QtNodes::NodeData;
using QtNodes::NodeImp;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextDisplayDataModel : public NodeImp {
  Q_OBJECT

public:
  TextDisplayDataModel();

  virtual ~TextDisplayDataModel() { delete _label; }

public:
  static QString Name() { return QString("TextDisplayDataModel"); }

  QString name() const override { return TextDisplayDataModel::Name(); }

  QWidget *embeddedWidget() override { return _label; }

private:
  QLabel *_label;
};
