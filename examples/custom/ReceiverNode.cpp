// ReceiverNode.cpp

#include "ReceiverNode.hpp"
#include "FormData.hpp"
#include "Receiver.hpp"

ReceiverNode::ReceiverNode()
    : widget_{new Receiver}
    , validationState_{QtNodes::NodeValidationState::Warning}
    , validationMessage_{"Data not set"} {
  QtNodes::Port port;
  port.caption        = "in port";
  port.captionVisible = false;
  port.dataType       = FormData().type();
  port.handle         = [this](std::shared_ptr<QtNodes::NodeData> data) {
    if (auto info = reinterpret_cast<FormData *>(data.get())) {
      widget_->setInfo(info->data());
      validationState_   = QtNodes::NodeValidationState::Valid;
      validationMessage_ = "";
    } else {
      widget_->setInfo(QString{});
      validationState_   = QtNodes::NodeValidationState::Warning;
      validationMessage_ = "Data not set";
    }
    return nullptr;
  };
  addPort(QtNodes::PortType::In, 5, port);
}

ReceiverNode::~ReceiverNode() {
  delete widget_;
}

QString ReceiverNode::name() const {
  return "ReceiverNode";
}

QWidget *ReceiverNode::embeddedWidget() {
  return widget_;
}

bool ReceiverNode::resizable() const {
  return true;
}

QtNodes::NodeValidationState ReceiverNode::validationState() const {
  return validationState_;
}

QString ReceiverNode::validationMessage() const {
  return validationMessage_;
}
