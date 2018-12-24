// ReceiverNode.hpp

#pragma once

#include "nodes/NodeImp.hpp"

class Receiver;

class ReceiverNode final : public QtNodes::NodeImp {
  Q_OBJECT
public:
  ReceiverNode();
  ~ReceiverNode();
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
