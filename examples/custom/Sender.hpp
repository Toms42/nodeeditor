#ifndef _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_CUSTOM_SENDER_HPP
#define _HOME_LEVKOVITCH_PUBLIC_MY_GIT_NODEEDITOR_EXAMPLES_CUSTOM_SENDER_HPP

// Sender.hpp

#pragma once

#include <QWidget>

namespace Ui {
class Sender;
} // namespace Ui

class QStringListModel;

class Sender : public QWidget {
  Q_OBJECT
public:
  explicit Sender(QWidget *parent = nullptr);
  ~Sender() override;
  QString getInfo() const;

signals:
  void dataIsReady();
  void addPort(const QString &caption, unsigned int index);
  void removePort(unsigned int index);

private:
  Ui::Sender *      ui_;
  QStringListModel *portListModel_;
};

#endif
