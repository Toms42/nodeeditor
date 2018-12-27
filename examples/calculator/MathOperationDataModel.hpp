#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
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
class MathOperationDataModel : public NodeDataModel {
  Q_OBJECT

public:
  MathOperationDataModel();
  virtual ~MathOperationDataModel() {}

public:
  QWidget *embeddedWidget() override { return nullptr; }

  NodeValidationState validationState() const override;

  QString validationMessage() const override;

protected:
  virtual void compute() = 0;

protected:
  std::weak_ptr<DecimalData> _number1;
  std::weak_ptr<DecimalData> _number2;

  std::shared_ptr<DecimalData> _result;

  NodeValidationState modelValidationState = NodeValidationState::Warning;
  QString modelValidationError = QString("Missing or incorrect inputs");
};
