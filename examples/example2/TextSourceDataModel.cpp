#include "TextSourceDataModel.hpp"

TextSourceDataModel::TextSourceDataModel()
    : _lineEdit(new QLineEdit("Default Text")) {
  QtNodes::Port port;
  port.caption        = "out caption";
  port.captionVisible = true;
  port.dataType       = TextData().type();
  port.handle         = [this](std::shared_ptr<NodeData>) {
    return std::make_shared<TextData>(_lineEdit->text());
  };

  addPort(PortType::Out, 5, port);
  connect(_lineEdit,
          &QLineEdit::textEdited,
          this,
          &TextSourceDataModel::onTextEdited);
}

void TextSourceDataModel::onTextEdited(QString const &string) {
  Q_UNUSED(string);

  if (string == "add port") {
    QtNodes::Port port;
    port.caption        = "new caption";
    port.captionVisible = true;
    port.dataType       = TextData().type();
    port.handle         = [this](std::shared_ptr<NodeData>) {
      return std::make_shared<TextData>(_lineEdit->text());
    };

    addPort(PortType::Out, 7, port);
  } else if (string == "remove port") {
    removePort(PortType::Out, 7);
  }

  for (auto &i : ports(PortType::Out)) {
    emit dataUpdated(i);
  }
}
