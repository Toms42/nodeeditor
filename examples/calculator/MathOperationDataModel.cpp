#include "MathOperationDataModel.hpp"
#include "DecimalData.hpp"

MathOperationDataModel::MathOperationDataModel() {
  QtNodes::Port inPort1;
  inPort1.captionVisible = false;
  inPort1.dataType       = DecimalData().type();
  inPort1.outPolicy      = QtNodes::ConnectionPolicy::Many;
  inPort1.handle         = [this](std::shared_ptr<NodeData> data) {
    auto numberData = std::dynamic_pointer_cast<DecimalData>(data);

    _number1 = numberData;

    compute();
    return nullptr;
  };

  QtNodes::Port inPort2;
  inPort2.captionVisible = false;
  inPort2.dataType       = DecimalData().type();
  inPort2.outPolicy      = QtNodes::ConnectionPolicy::Many;
  inPort2.handle         = [this](std::shared_ptr<NodeData> data) {
    auto numberData = std::dynamic_pointer_cast<DecimalData>(data);

    _number2 = numberData;

    compute();
    return nullptr;
  };

  QtNodes::Port outPort;
  outPort.captionVisible = false;
  outPort.dataType       = DecimalData().type();
  outPort.outPolicy      = QtNodes::ConnectionPolicy::Many;
  outPort.handle         = [this](std::shared_ptr<NodeData>) {
    return std::static_pointer_cast<NodeData>(_result);
  };

  addPort(PortType::In, 1, inPort1);
  addPort(PortType::In, 2, inPort2);
  addPort(PortType::Out, 3, outPort);
}

NodeValidationState MathOperationDataModel::validationState() const {
  return modelValidationState;
}

QString MathOperationDataModel::validationMessage() const {
  return modelValidationError;
}
