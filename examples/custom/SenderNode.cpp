// SenderNode.cpp

#include "SenderNode.hpp"
#include "FormData.hpp"
#include "Sender.hpp"

SenderNode::SenderNode()
    : widget_{new Sender}
    , validationState_{QtNodes::NodeValidationState::Valid}
    , validationMessage_{""} {
  connect(widget_, &Sender::dataIsReady, this, [this]() {
    for (auto &i : ports(QtNodes::PortType::Out)) {
      emit dataUpdated(i);
    }
  });

  connect(widget_, &Sender::addPort, this, &SenderNode::addPort);
  connect(widget_, &Sender::removePort, this, &SenderNode::removePort);
}

SenderNode::~SenderNode() {
  delete widget_;
}

QString SenderNode::name() const {
  return "SenderNode";
}

QWidget *SenderNode::embeddedWidget() {
  return widget_;
}

QtNodes::NodeValidationState SenderNode::validationState() const {
  return validationState_;
}

QString SenderNode::validationMessage() const {
  return validationMessage_;
}

bool SenderNode::addPort(const QString &caption, unsigned int index) {
  QtNodes::Port port;
  port.caption        = caption;
  port.captionVisible = true;
  port.dataType       = FormData().type();
  port.outPolicy      = QtNodes::ConnectionPolicy::One;
  port.handle         = [this](std::shared_ptr<QtNodes::NodeData>) {
    return std::make_shared<FormData>(widget_->getInfo());
    emit
  };

  return QtNodes::NodeDataModel::addPort(QtNodes::PortType::Out, index, port);
};

bool SenderNode::SenderNode::removePort(unsigned int index) {
  return QtNodes::NodeDataModel::removePort(QtNodes::PortType::Out, index);
}
