// CustomLabel.hpp

#pragma once

#include <QLabel>

class CustomLabel : public QLabel {
  Q_OBJECT
public:
  CustomLabel(QWidget *parent = nullptr);

protected:
  void contextMenuEvent(QContextMenuEvent *);

signals:
  void toModel();
};
