// SenderNode.hpp

#pragma once

#include "nodes/NodeDataModel.hpp"

class Sender;

class SenderNode final : public QtNodes::NodeDataModel {
  Q_OBJECT
public:
  SenderNode();
  ~SenderNode() override;
  QString                      name() const override;
  QWidget *                    embeddedWidget() override;
  QtNodes::NodeValidationState validationState() const override;
  QString                      validationMessage() const override;

private:
  QJsonObject save() const override;
  void        restore(const QJsonObject &jsonObj) override;

private slots:
  bool addPort(const QString &caption, unsigned int index);
  bool removePort(unsigned int index);

private:
  Sender *widget_;

  QtNodes::NodeValidationState validationState_;
  QString                      validationMessage_;
};
