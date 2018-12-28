// CompositeGeometry.hpp

#pragma once

#include "NodeIndex.hpp"
#include "PortType.hpp"
#include <QFont>
#include <QFontMetrics>
#include <QPointF>
#include <QRectF>
#include <QTransform>

namespace QtNodes {
class NodeComposite;

class CompositeGeometry {
public:
  CompositeGeometry(NodeComposite &obj);
  virtual ~CompositeGeometry() = default;

  unsigned int height() const;

  void setHeight(unsigned int h);

  unsigned int width() const;

  void setWidth(unsigned int w);

  unsigned int entryHeight() const;

  void setEntryHeight(unsigned int h);

  unsigned int entryWidth() const;

  void setEntryWidth(unsigned int w);

  unsigned int spacing() const;

  void setSpacing(unsigned int s);

  bool hovered() const;

  void setHovered(unsigned int h);

  virtual QPointF const &draggingPos() const;

  virtual void setDraggingPosition(QPointF const &pos);

  virtual QRectF entryBoundingRect() const;

  virtual QRectF boundingRect() const;

  /// Updates size unconditionally
  virtual void recalculateSize() const = 0;

  /// Updates size if the QFontMetrics is changed
  virtual void recalculateSize(QFont const &font) const;

  virtual QRect resizeRect() const;

  void prepareGeometryChange() const;

protected:
  NodeComposite &obj_;

  // some variables are mutable because
  // we need to change drawing metrics
  // corresponding to fontMetrics
  // but this doesn't change constness of Node

  mutable unsigned int width_;
  mutable unsigned int height_;
  unsigned int         entryWidth_;
  mutable unsigned int entryHeight_;
  unsigned int         spacing_;

  bool hovered_;

  QPointF draggingPos_;

  mutable QFontMetrics fontMetrics_;
  mutable QFontMetrics boldFontMetrics_;
};
}; // namespace QtNodes
