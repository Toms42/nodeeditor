// CompositeGeometry.cpp

#include "CompositeGeometry.hpp"
#include "NodeComposite.hpp"
#include "StyleCollection.hpp"

namespace QtNodes {
CompositeGeometry::CompositeGeometry(NodeComposite &obj)
    : obj_{obj}
    , width_{100}
    , height_{150}
    , entryWidth_{0}
    , entryHeight_{0}
    , spacing_{20}
    , hovered_{false}
    , draggingPos_{-1000, -1000}
    , fontMetrics_{QFont{}}
    , boldFontMetrics_{QFont{}} {
  QFont f;
  f.setBold(true);

  boldFontMetrics_ = QFontMetrics(f);
}

unsigned int CompositeGeometry::height() const {
  return height_;
}

void CompositeGeometry::setHeight(unsigned int h) {
  prepareGeometryChange();
  height_ = h;
}

unsigned int CompositeGeometry::width() const {
  return width_;
}

void CompositeGeometry::setWidth(unsigned int w) {
  prepareGeometryChange();
  width_ = w;
}

unsigned int CompositeGeometry::entryHeight() const {
  return entryHeight_;
}

void CompositeGeometry::setEntryHeight(unsigned int h) {
  entryHeight_ = h;
}

unsigned int CompositeGeometry::entryWidth() const {
  return entryWidth_;
}

void CompositeGeometry::setEntryWidth(unsigned int w) {
  entryWidth_ = w;
}

unsigned int CompositeGeometry::spacing() const {
  return spacing_;
}

void CompositeGeometry::setSpacing(unsigned int s) {
  spacing_ = s;
}

bool CompositeGeometry::hovered() const {
  return hovered_;
}

void CompositeGeometry::setHovered(unsigned int h) {
  hovered_ = h;
}

QPointF const &CompositeGeometry::draggingPos() const {
  return draggingPos_;
}

void CompositeGeometry::setDraggingPosition(QPointF const &pos) {
  draggingPos_ = pos;
}

QRectF CompositeGeometry::entryBoundingRect() const {
  double const addon = 0.0;

  return QRectF(0 - addon,
                0 - addon,
                entryWidth() + 2 * addon,
                entryHeight() + 2 * addon);
}

QRectF CompositeGeometry::boundingRect() const {
  auto const &nodeStyle = StyleCollection::nodeStyle();

  double addon = 4 * nodeStyle.ConnectionPointDiameter;

  return QRectF(
      0 - addon, 0 - addon, width() + 2 * addon, height() + 2 * addon);
}

QRect CompositeGeometry::resizeRect() const {
  unsigned int rectSize = 7;

  return QRect(width() - rectSize, height() - rectSize, rectSize, rectSize);
}

void CompositeGeometry::prepareGeometryChange() const {
  obj_.prepareGeometryChange();
}

void CompositeGeometry::recalculateSize(QFont const &font) const {
  QFontMetrics fontMetrics(font);
  QFont        boldFont = font;

  boldFont.setBold(true);

  QFontMetrics boldFontMetrics(boldFont);

  if (boldFontMetrics_ != boldFontMetrics) {
    fontMetrics_     = fontMetrics;
    boldFontMetrics_ = boldFontMetrics;

    recalculateSize();
  }
}

} // namespace QtNodes
