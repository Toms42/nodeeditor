#include "NumberSourceDataModel.hpp"
#include "DecimalData.hpp"
#include <QtCore/QJsonValue>
#include <QtGui/QDoubleValidator>

NumberSourceDataModel::NumberSourceDataModel()
    : _lineEdit(new QLineEdit()) {
  _lineEdit->setValidator(new QDoubleValidator(_lineEdit));

  _lineEdit->setMaximumSize(_lineEdit->sizeHint());

  connect(_lineEdit,
          &QLineEdit::textChanged,
          this,
          &NumberSourceDataModel::onTextEdited);

  _lineEdit->setText("0.0");

  QtNodes::Port outPort;
  outPort.dataType = DecimalData().type();
  outPort.handle   = [this](std::shared_ptr<NodeData>) { return _number; };

  addPort(PortType::Out, 1, outPort);
}

NumberSourceDataModel::~NumberSourceDataModel() {
  delete _lineEdit;
}

QJsonObject NumberSourceDataModel::save() const {
  QJsonObject modelJson = NodeDataModel::save();

  if (_number)
    modelJson["number"] = QString::number(_number->number());

  return modelJson;
}

void NumberSourceDataModel::restore(QJsonObject const &p) {
  QJsonValue v = p["number"];

  if (!v.isUndefined()) {
    QString strNum = v.toString();

    bool   ok;
    double d = strNum.toDouble(&ok);
    if (ok) {
      _number = std::make_shared<DecimalData>(d);
      _lineEdit->setText(strNum);
    }
  }
}

void NumberSourceDataModel::onTextEdited(QString const &string) {
  Q_UNUSED(string);

  bool ok = false;

  double number = _lineEdit->text().toDouble(&ok);

  if (ok) {
    _number = std::make_shared<DecimalData>(number);

    for (auto &i : this->ports(PortType::Out)) {
      emit dataUpdated(i);
    }
  } else {
    for (auto &i : this->ports(PortType::Out)) {
      emit dataInvalidated(i);
    }
  }
}
