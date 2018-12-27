#include "ModuloModel.hpp"
#include "IntegerData.hpp"
#include <QtGui/QDoubleValidator>

// bool portCaptionVisible(PortType, PortIndex) const override { return true;
// }

// QString portCaption(PortType portType, PortIndex portIndex) const override
// {
//  switch (portType) {
//  case PortType::In:
//    if (portIndex == 0)
//      return QStringLiteral("Dividend");
//    else if (portIndex == 1)
//      return QStringLiteral("Divisor");

//    break;

//  case PortType::Out:
//    return QStringLiteral("Result");

//  default:
//    break;
//  }
//  return QString();
//}

ModuloModel::ModuloModel() {
  QtNodes::Port inPort1;
  inPort1.captionVisible = false;
  inPort1.dataType       = IntegerData().type();
  inPort1.outPolicy      = QtNodes::ConnectionPolicy::Many;
  inPort1.handle         = [this](std::shared_ptr<NodeData> data) {
    auto numberData = std::dynamic_pointer_cast<IntegerData>(data);
    _number1        = numberData;
    {
      auto n1 = _number1.lock();
      auto n2 = _number2.lock();

      if (n2 && (n2->number() == 0.0)) {
        modelValidationState = NodeValidationState::Error;
        modelValidationError = QStringLiteral("Division by zero error");
        _result.reset();
      } else if (n1 && n2) {
        modelValidationState = NodeValidationState::Valid;
        modelValidationError = QString();
        _result = std::make_shared<IntegerData>(n1->number() % n2->number());
      } else {
        modelValidationState = NodeValidationState::Warning;
        modelValidationError = QStringLiteral("Missing or incorrect inputs");
        _result.reset();
      }

      for (auto &i : ports(PortType::Out)) {
        emit dataUpdated(i);
      }
    }

    return nullptr;
  };

  QtNodes::Port inPort2;
  inPort2.captionVisible = false;
  inPort2.dataType       = IntegerData().type();
  inPort2.outPolicy      = QtNodes::ConnectionPolicy::Many;
  inPort2.handle         = [this](std::shared_ptr<NodeData> data) {
    auto numberData = std::dynamic_pointer_cast<IntegerData>(data);
    _number2        = numberData;
    {
      auto n1 = _number1.lock();
      auto n2 = _number2.lock();

      if (n2 && (n2->number() == 0.0)) {
        modelValidationState = NodeValidationState::Error;
        modelValidationError = QStringLiteral("Division by zero error");
        _result.reset();
      } else if (n1 && n2) {
        modelValidationState = NodeValidationState::Valid;
        modelValidationError = QString();
        _result = std::make_shared<IntegerData>(n1->number() % n2->number());
      } else {
        modelValidationState = NodeValidationState::Warning;
        modelValidationError = QStringLiteral("Missing or incorrect inputs");
        _result.reset();
      }

      for (auto &i : ports(PortType::Out)) {
        emit dataUpdated(i);
      }
    }

    return nullptr;
  };

  QtNodes::Port outPort;
  outPort.captionVisible = false;
  outPort.dataType       = IntegerData().type();
  outPort.outPolicy      = QtNodes::ConnectionPolicy::Many;
  outPort.handle = [this](std::shared_ptr<NodeData>) { return _result; };

  addPort(PortType::In, 1, inPort1);
  addPort(PortType::In, 2, inPort2);
  addPort(PortType::Out, 3, outPort);
}

QJsonObject ModuloModel::save() const {
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}

NodeValidationState ModuloModel::validationState() const {
  return modelValidationState;
}

QString ModuloModel::validationMessage() const {
  return modelValidationError;
}
