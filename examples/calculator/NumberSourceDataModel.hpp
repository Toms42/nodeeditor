#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>
#include <iostream>
#include <nodes/NodeDataModel.hpp>

class DecimalData;

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NumberSourceDataModel : public NodeDataModel {
  Q_OBJECT

public:
  NumberSourceDataModel();

  virtual ~NumberSourceDataModel();

public:
  QString name() const override { return QStringLiteral("NumberSource"); }

public:
  QJsonObject save() const override;

  void restore(QJsonObject const &p) override;

public:
  QWidget *embeddedWidget() override { return _lineEdit; }

private slots:

  void onTextEdited(QString const &string);

private:
  std::shared_ptr<DecimalData> _number;

  QLineEdit *_lineEdit;
};
