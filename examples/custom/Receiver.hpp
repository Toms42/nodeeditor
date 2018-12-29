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

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;

signals:
  void toModel();

private:
  Ui::Receiver *ui_;
};
