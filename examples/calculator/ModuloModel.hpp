#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>
#include <iostream>
#include <nodes/NodeDataModel.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

class IntegerData;

class ModuloModel : public NodeDataModel {
  Q_OBJECT

public:
  ModuloModel();

  virtual ~ModuloModel() = default;

public:
  QString name() const override { return QStringLiteral("Modulo"); }

public:
  QJsonObject save() const override;

public:
  QWidget *embeddedWidget() override { return nullptr; }

  NodeValidationState validationState() const override;

  QString validationMessage() const override;

private:
  std::weak_ptr<IntegerData> _number1;
  std::weak_ptr<IntegerData> _number2;

  std::shared_ptr<IntegerData> _result;

  NodeValidationState modelValidationState = NodeValidationState::Warning;
  QString modelValidationError = QStringLiteral("Missing or incorrect inputs");
};
