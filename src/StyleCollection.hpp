#pragma once

#include "ConnectionStyle.hpp"
#include "FlowViewStyle.hpp"
#include "NodeStyle.hpp"

#include <QObject>

namespace QtNodes {

class StyleCollection : public QObject {
  Q_OBJECT

public:
  static StyleCollection &instance();

  static NodeStyle const &nodeStyle();

  static ConnectionStyle const &connectionStyle();

  static FlowViewStyle const &flowViewStyle();

public:
  static void setNodeStyle(NodeStyle);

  static void setConnectionStyle(ConnectionStyle);

  static void setFlowViewStyle(FlowViewStyle);

  /**\brief when we change style - then boundingRect of geometry classes can
   * change, so, we have to send message to they about change style
   */
signals:
  void nodeStyleChanged();
  void connectionStyleChanged();
  void flowViweStyleChanged();

private:
  StyleCollection() = default;

  StyleCollection(StyleCollection const &) = delete;

  StyleCollection &operator=(StyleCollection const &) = delete;

private:
  NodeStyle _nodeStyle;

  ConnectionStyle _connectionStyle;

  FlowViewStyle _flowViewStyle;
};
} // namespace QtNodes
