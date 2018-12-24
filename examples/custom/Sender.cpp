// Sender.cpp

#include "Sender.hpp"
#include "ui_Sender.h"
#include <QStringListModel>

Sender::Sender(QWidget *parent)
    : QWidget{parent}
    , ui_{new Ui::Sender}
    , portListModel_{new QStringListModel(this)} {
  ui_->setupUi(this);
  ui_->textEdit->setPlaceholderText("input text");
  ui_->portView->setModel(portListModel_);
  connect(ui_->textEdit, &QTextEdit::textChanged, this, &Sender::dataIsReady);
  connect(ui_->addPort, &QPushButton::released, this, [this]() {
    static int index{};
    emit       addPort(QString::number(index), index);

    if (portListModel_->insertRow(portListModel_->rowCount())) {
      portListModel_->setData(
          portListModel_->index(portListModel_->rowCount() - 1),
          QString::number(index));
    }
    ++index;
  });
  connect(ui_->removePort, &QPushButton::released, this, [this]() {
    auto curModelIndex = ui_->portView->currentIndex();
    auto index         = curModelIndex.data().toString().toInt();

    if (portListModel_->removeRow(curModelIndex.row())) {
      emit removePort(index);
    }
  });
}

Sender::~Sender() {
  delete ui_;
}

QString Sender::getInfo() const {
  return ui_->textEdit->toPlainText();
}
