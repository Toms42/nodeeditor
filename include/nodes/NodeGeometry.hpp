#pragma once

#include "Export.hpp"
#include "NodeIndex.hpp"
#include "PortType.hpp"
#include "memory.hpp"
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtGui/QFontMetrics>
#include <QtGui/QTransform>

namespace QtNodes {

class NodeComposite;

class NODE_EDITOR_PUBLIC NodeGeometry final : public QObject {
public:
  NodeGeometry(NodeIndex const &index, NodeComposite &obj);

public:
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

  unsigned int nSources() const;

  unsigned int nSinks() const;

  QPointF const &draggingPos() const;

  void setDraggingPosition(QPointF const &pos);

public:
  QRectF entryBoundingRect() const;

  QRectF boundingRect() const;

  /// Updates size unconditionally
  void recalculateSize() const;

  /// Updates size if the QFontMetrics is changed
  void recalculateSize(QFont const &font) const;

  // TODO removed default QTransform()
  QPointF portScenePosition(PortIndex         index,
                            PortType          portType,
                            QTransform const &t = QTransform()) const;

  PortIndex checkHitScenePoint(PortType          portType,
                               QPointF           point,
                               QTransform const &t = QTransform()) const;

  QRect resizeRect() const;

  /// Returns the position of a widget on the Node surface
  QPointF widgetPosition() const;

  unsigned int validationHeight() const;

  unsigned int validationWidth() const;

  static QPointF
  calculateNodePositionBetweenNodePorts(PortIndex            targetPortIndex,
                                        PortType             targetPort,
                                        NodeComposite const &targetNode,
                                        PortIndex            sourcePortIndex,
                                        PortType             sourcePort,
                                        NodeComposite const &sourceNode,
                                        NodeGeometry const & newNodeGeom);

  void prepareGeometryChange() const;

private:
  unsigned int captionHeight() const;

  unsigned int captionWidth() const;

  unsigned int portWidth(PortType portType) const;

private:
  NodeComposite &_obj;

  // some variables are mutable because
  // we need to change drawing metrics
  // corresponding to fontMetrics
  // but this doesn't change constness of Node

  mutable unsigned int _width;
  mutable unsigned int _height;
  unsigned int         _entryWidth;
  mutable unsigned int _inputPortWidth;
  mutable unsigned int _outputPortWidth;
  mutable unsigned int _entryHeight;
  unsigned int         _spacing;

  bool _hovered;

  QPointF _draggingPos;

  NodeIndex _nodeIndex;

  mutable QFontMetrics _fontMetrics;
  mutable QFontMetrics _boldFontMetrics;
};
} // namespace QtNodes
