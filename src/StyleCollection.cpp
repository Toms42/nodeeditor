#include "StyleCollection.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::FlowViewStyle;
using QtNodes::NodeStyle;
using QtNodes::StyleCollection;

NodeStyle const &StyleCollection::nodeStyle() { return instance()._nodeStyle; }

ConnectionStyle const &StyleCollection::connectionStyle() {
  return instance()._connectionStyle;
}

FlowViewStyle const &StyleCollection::flowViewStyle() {
  return instance()._flowViewStyle;
}

void StyleCollection::setNodeStyle(NodeStyle nodeStyle) {
  instance()._nodeStyle = nodeStyle;
  emit instance().nodeStyleChanged();
}

void StyleCollection::setConnectionStyle(ConnectionStyle connectionStyle) {
  instance()._connectionStyle = connectionStyle;
  emit instance().connectionStyleChanged();
}

void StyleCollection::setFlowViewStyle(FlowViewStyle flowViewStyle) {
  instance()._flowViewStyle = flowViewStyle;
  emit instance().flowViweStyleChanged();
}

StyleCollection &StyleCollection::instance() {
  static StyleCollection collection;

  return collection;
}
