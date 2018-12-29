#include "NumberDisplayDataModel.hpp"
#include "DecimalData.hpp"

NumberDisplayDataModel::NumberDisplayDataModel()
    : _label(new QLabel()) {
  _label->setMargin(3);

  QtNodes::Port inPort;
  inPort.dataType = DecimalData().type();
  inPort.handle   = [this](std::shared_ptr<NodeData> data) {
    auto numberData = std::dynamic_pointer_cast<DecimalData>(data);

    if (numberData) {
      modelValidationState = NodeValidationState::Valid;
      modelValidationError = QString();
      _label->setText(numberData->numberAsText());
    } else {
      modelValidationState = NodeValidationState::Warning;
      modelValidationError = QStringLiteral("Missing or incorrect inputs");
      _label->clear();
    }

    _label->adjustSize();

    return nullptr;
  };

  addPort(PortType::In, 1, inPort);
}

NumberDisplayDataModel::~NumberDisplayDataModel() {
  _label->deleteLater();
}

NodeValidationState NumberDisplayDataModel::validationState() const {
  return modelValidationState;
}

QString NumberDisplayDataModel::validationMessage() const {
  return modelValidationError;
}
