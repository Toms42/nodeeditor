// CustomLabel.cpp

#include "CustomLabel.hpp"

CustomLabel::CustomLabel(QWidget *parent)
    : QLabel{parent} {}

void CustomLabel::contextMenuEvent(QContextMenuEvent *) {
  emit toModel();
}
