// ReceiverNode.hpp

#pragma once

#include "nodes/NodeDataModel.hpp"

class Receiver;
class CustomLabel;

class ReceiverNode final : public QtNodes::NodeDataModel {
  Q_OBJECT
public:
  ReceiverNode();
  ~ReceiverNode() override;
  QString                      name() const override;
  QWidget *                    embeddedWidget() override;
  QtNodes::NodeValidationState validationState() const override;
  QString                      validationMessage() const override;

private:
  QJsonObject save() const override;
  void        restore(const QJsonObject &obj) override;

private:
  Receiver *   widget_;
  CustomLabel *label_;

  QtNodes::NodeValidationState validationState_;
  QString                      validationMessage_;
  bool                         check_;
};
