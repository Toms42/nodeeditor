// Receiver.hpp

#pragma once

#include <QWidget>

namespace Ui {
class Receiver;
};

class Receiver : public QWidget {
  Q_OBJECT
public:
  explicit Receiver(QWidget *parent = nullptr);
  ~Receiver();
  void setInfo(const QString &data);

private:
  Ui::Receiver *ui_;
};
