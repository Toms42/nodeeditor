#include "TextDisplayDataModel.hpp"

TextDisplayDataModel::TextDisplayDataModel()
    : _label(new QLabel("Resulting Text")) {
  _label->setMargin(3);
  QtNodes::Port port;
  port.caption = "in caption";
  port.captionVisible = true;
  port.dataType = TextData().type();
  port.handle = [this](std::shared_ptr<NodeData> in) {
    auto data = std::dynamic_pointer_cast<TextData>(in);
    if (data) {
      _label->setText(data->text());
    } else {
      _label->clear();
    }
    return std::shared_ptr<NodeData>();
  };
  addPort(PortType::In, 3, port);
}
