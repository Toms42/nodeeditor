// Sender.hpp

#pragma once

#include <QWidget>

namespace Ui {
class Sender;
};

class QStringListModel;

class Sender : public QWidget {
  Q_OBJECT
public:
  explicit Sender(QWidget *parent = nullptr);
  ~Sender();
  QString getInfo() const;

signals:
  void dataIsReady();
  void addPort(const QString &caption, unsigned int index);
  void removePort(unsigned int index);

private:
  Ui::Sender *      ui_;
  QStringListModel *portListModel_;
};
