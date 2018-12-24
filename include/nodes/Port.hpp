// Port.hpp

#pragma once

#include "Export.hpp"
#include "types.hpp"
#include <QString>
#include <functional>
#include <memory>

namespace QtNodes {

class NodeData;

struct NODE_EDITOR_PUBLIC Port final {
  Port()
      : captionVisible{true}
      , policy{ConnectionPolicy::Many} {}

  QString               caption;
  bool                  captionVisible;
  QtNodes::NodeDataType dataType;

  /**\brief by default Many
   */
  ConnectionPolicy policy;

  // this function process data (in or out). For In port you can ignore retunr
  // value, and for Out port you can ignore argument (nullptr by default)
  std::function<std::shared_ptr<QtNodes::NodeData>(
      std::shared_ptr<QtNodes::NodeData>)>
      handle;
};
}; // namespace QtNodes
