// ReceiverNode.cpp

#include "ReceiverNode.hpp"
#include "CustomLabel.hpp"
#include "FormData.hpp"
#include "Receiver.hpp"

ReceiverNode::ReceiverNode()
    : widget_{new Receiver}
    , label_{new CustomLabel}
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

  connect(widget_, &Receiver::toModel, this, [this]() {
    check_ = !check_;
    emit widgetChanged();
  });

  connect(label_, &CustomLabel::toModel, this, [this]() {
    check_ = !check_;
    emit widgetChanged();
  });

  label_->setText("this is another widget");
}

ReceiverNode::~ReceiverNode() {
  widget_->deleteLater();
  delete label_;
}

QString ReceiverNode::name() const {
  return "ReceiverNode";
}

QWidget *ReceiverNode::embeddedWidget() {
  if (!check_) {
    return widget_;
  } else {
    return label_;
  }
}

QtNodes::NodeValidationState ReceiverNode::validationState() const {
  return validationState_;
}

QString ReceiverNode::validationMessage() const {
  return validationMessage_;
}

QJsonObject ReceiverNode::save() const {
  return NodeDataModel::save();
}

void ReceiverNode::restore(const QJsonObject &) {}
