#ifndef _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_CUSTOM_RECEIVERNODE_HPP
#define _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_CUSTOM_RECEIVERNODE_HPP

// ReceiverNode.hpp

#pragma once

#include "nodes/NodeDataModel.hpp"

class Receiver;

class ReceiverNode final : public QtNodes::NodeDataModel {
  Q_OBJECT
public:
  ReceiverNode();
  ~ReceiverNode() override;
  QString                      name() const override;
  QWidget *                    embeddedWidget() override;
  bool                         resizable() const override;
  QtNodes::NodeValidationState validationState() const override;
  QString                      validationMessage() const override;

private:
  Receiver *widget_;

  QtNodes::NodeValidationState validationState_;
  QString                      validationMessage_;
};

#endif
