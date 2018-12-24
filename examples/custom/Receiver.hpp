#ifndef _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_CUSTOM_RECEIVER_HPP
#define _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_CUSTOM_RECEIVER_HPP

// Receiver.hpp

#pragma once

#include <QWidget>

namespace Ui {
class Receiver;
} // namespace Ui

class Receiver : public QWidget {
  Q_OBJECT
public:
  explicit Receiver(QWidget *parent = nullptr);
  ~Receiver() override;
  void setInfo(const QString &data);

private:
  Ui::Receiver *ui_;
};

#endif
