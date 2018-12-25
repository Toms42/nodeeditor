// FormData.hpp

#pragma once

#include "nodes/NodeData.hpp"

class FormData : public QtNodes::NodeData {
public:
  FormData() = default;
  explicit FormData(const QString &data)
      : data_{data} {};

  QtNodes::NodeDataType type() const override {
    return QtNodes::NodeDataType{"Form", "text"};
  };

  QString data() const { return data_; };

private:
  QString data_;
};
