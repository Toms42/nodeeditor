#pragma once

#include "DecimalData.hpp"
#include "MathOperationDataModel.hpp"
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <nodes/NodeDataModel.hpp>

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class SubtractionModel : public MathOperationDataModel {
public:
  virtual ~SubtractionModel() {}

public:
  // virtual QString portCaption(PortType  portType,
  //                            PortIndex portIndex) const override {
  //  switch (portType) {
  //  case PortType::In:
  //    if (portIndex == 0)
  //      return QStringLiteral("Minuend");
  //    else if (portIndex == 1)
  //      return QStringLiteral("Subtrahend");

  //    break;

  //  case PortType::Out:
  //    return QStringLiteral("Result");

  //  default:
  //    break;
  //  }
  //  return QString();
  //}

  QString name() const override { return QStringLiteral("Subtraction"); }

private:
  void compute() override {
    auto n1 = _number1.lock();
    auto n2 = _number2.lock();

    if (n1 && n2) {
      modelValidationState = NodeValidationState::Valid;
      modelValidationError = QString();
      _result = std::make_shared<DecimalData>(n1->number() - n2->number());
    } else {
      modelValidationState = NodeValidationState::Warning;
      modelValidationError = QStringLiteral("Missing or incorrect inputs");
      _result.reset();
    }

    for (auto &i : ports(PortType::Out)) {
      emit dataUpdated(i);
    }
  }
};
