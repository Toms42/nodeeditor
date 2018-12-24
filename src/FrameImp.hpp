// Frame.hpp

#pragma once

#include "NodeImp.hpp"
#include <QWidget>
#include <iostream>

namespace QtNodes {

class FrameImp final : public NodeImp {
public:
  FrameImp()
      : widget_{new QWidget} {}
  ~FrameImp() { delete widget_; }
  QString  name() const override { return "Frame"; }
  QWidget *embeddedWidget() override { return widget_; }
  bool     resizable() const override { return true; }

private:
  QWidget *widget_;
};
}; // namespace QtNodes
