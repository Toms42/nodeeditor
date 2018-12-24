#pragma once

#include "TextData.hpp"
#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>
#include <iostream>
#include <nodes/NodeImp.hpp>

using QtNodes::NodeData;
using QtNodes::NodeImp;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextSourceDataModel : public NodeImp {
  Q_OBJECT

public:
  TextSourceDataModel();

  virtual ~TextSourceDataModel() { delete _lineEdit; }

public:
  static QString Name() { return QString("TextSourceDataModel"); }

  QString name() const override { return TextSourceDataModel::Name(); }

public:
  QWidget *embeddedWidget() override { return _lineEdit; }

private slots:

  void onTextEdited(QString const &string);

private:
  QLineEdit *_lineEdit;
};
