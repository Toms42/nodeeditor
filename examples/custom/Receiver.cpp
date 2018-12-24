// Receiver.cpp

#include "Receiver.hpp"
#include "ui_Receiver.h"

Receiver::Receiver(QWidget *parent) : QWidget{parent}, ui_{new Ui::Receiver} {
  ui_->setupUi(this);
}

Receiver::~Receiver() { delete ui_; }

void Receiver::setInfo(const QString &data) { ui_->textBrowser->setText(data); }
