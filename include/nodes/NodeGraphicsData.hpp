// NodeGraphicsData.hpp

#pragma once

#include <QPointF>
#include <QSize>
#include <QUuid>

struct NodeGraphicsData final {
  NodeGraphicsData(QPointF pos_, QSize size_, QUuid parent_)
      : pos{pos_}
      , size{size_}
      , parent{parent_} {}
  QPointF pos;
  QSize   size;
  QUuid   parent;
};
