#ifndef _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_CUSTOM_SENDERNODE_HPP
#define _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_CUSTOM_SENDERNODE_HPP

// SenderNode.hpp

#pragma once

#include "nodes/NodeImp.hpp"

class Sender;

class SenderNode final : public QtNodes::NodeImp {
  Q_OBJECT
public:
  SenderNode();
  ~SenderNode() override;
  QString                      name() const override;
  QWidget *                    embeddedWidget() override;
  bool                         resizable() const override;
  QtNodes::NodeValidationState validationState() const override;
  QString                      validationMessage() const override;

private slots:
  bool addPort(const QString &caption, unsigned int index);
  bool removePort(unsigned int index);

private:
  Sender *widget_;

  QtNodes::NodeValidationState validationState_;
  QString                      validationMessage_;
};

#endif
