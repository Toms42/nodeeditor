// FrameDataModel.hpp

#pragma once

#include "NodeDataModel.hpp"
#include <QWidget>
#include <iostream>

namespace QtNodes {

class FrameImp final : public NodeDataModel {
public:
  FrameImp()
      : widget_{new QWidget} {}
  ~FrameImp() override { delete widget_; }
  QString  name() const override { return "Frame"; }
  QWidget *embeddedWidget() override { return widget_; }
  bool     resizable() const override { return true; }

private:
  QWidget *widget_;
};
}; // namespace QtNodes
