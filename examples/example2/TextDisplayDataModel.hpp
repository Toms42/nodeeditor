#pragma once

#include "TextData.hpp"
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <iostream>
#include <nodes/NodeImp.hpp>

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
