#include "NodeGeometry.hpp"
#include "FlowSceneModel.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeIndex.hpp"
#include "PortType.hpp"
#include "StyleCollection.hpp"
#include <QWidget>
#include <cmath>
#include <iostream>

#include "checker.hpp"

namespace QtNodes {

unsigned int NodeGeometry::height() const { return _height; }

void NodeGeometry::setHeight(unsigned int h) {
  _obj.prepareGeometryChange();
  _height = h;
}

unsigned int NodeGeometry::width() const { return _width; }

void NodeGeometry::setWidth(unsigned int w) {
  _obj.prepareGeometryChange();
  _width = w;
}

unsigned int NodeGeometry::entryHeight() const { return _entryHeight; }

void NodeGeometry::setEntryHeight(unsigned int h) { _entryHeight = h; }

unsigned int NodeGeometry::entryWidth() const { return _entryWidth; }

void NodeGeometry::setEntryWidth(unsigned int w) { _entryWidth = w; }

unsigned int NodeGeometry::spacing() const { return _spacing; }

void NodeGeometry::setSpacing(unsigned int s) { _spacing = s; }

bool NodeGeometry::hovered() const { return _hovered; }

void NodeGeometry::setHovered(unsigned int h) { _hovered = h; }

QPointF const &NodeGeometry::draggingPos() const { return _draggingPos; }

void NodeGeometry::setDraggingPosition(QPointF const &pos) {
  _draggingPos = pos;
}

NodeGeometry::NodeGeometry(const NodeIndex &index, NodeGraphicsObject &obj)
    : _obj{obj}, _width(100), _height(150), _entryWidth(0), _inputPortWidth(70),
      _outputPortWidth(70), _entryHeight(0), _spacing(20), _hovered(false),
      _draggingPos(-1000, -1000), _nodeIndex(index), _fontMetrics(QFont()),
      _boldFontMetrics(QFont()) {
  QFont f;
  f.setBold(true);

  _boldFontMetrics = QFontMetrics(f);

  // TODO if style can change dynamicly, so it have to be unkomment
  // connect(&StyleCollection::instance(), &StyleCollection::nodeStyleChanged,
  //        this, [&]() {
  //          _obj.prepareGeometryChange();
  //          _obj.boundingRect();
  //        });
}

unsigned int NodeGeometry::nSources() const {
  return _nodeIndex.model()->nodePortCount(_nodeIndex, PortType::Out);
}

unsigned int NodeGeometry::nSinks() const {
  return _nodeIndex.model()->nodePortCount(_nodeIndex, PortType::In);
}

QRectF NodeGeometry::entryBoundingRect() const {
  double const addon = 0.0;

  return QRectF(0 - addon, 0 - addon, _entryWidth + 2 * addon,
                _entryHeight + 2 * addon);
}

QRectF NodeGeometry::boundingRect() const {
  auto const &nodeStyle = StyleCollection::nodeStyle();

  double addon = 4 * nodeStyle.ConnectionPointDiameter;

  return QRectF(0 - addon, 0 - addon, _width + 2 * addon, _height + 2 * addon);
}

void NodeGeometry::recalculateSize() const {
  _obj.prepareGeometryChange();

  _entryHeight = _fontMetrics.height();

  {
    unsigned int maxNumOfEntries = std::max(nSinks(), nSources());
    unsigned int step = _entryHeight + _spacing;
    _height = step * maxNumOfEntries;
  }

  if (auto w = _nodeIndex.model()->nodeWidget(_nodeIndex)) {
    _height = std::max(_height, static_cast<unsigned>(w->height()));
  }

  _height += captionHeight();

  _inputPortWidth = portWidth(PortType::In);
  _outputPortWidth = portWidth(PortType::Out);

  _width = _inputPortWidth + _outputPortWidth + 2 * _spacing;

  if (auto w = _nodeIndex.model()->nodeWidget(_nodeIndex)) {
    _width += w->width();
  }

  _width = std::max(_width, captionWidth());

  if (_nodeIndex.model()->nodeValidationState(_nodeIndex) !=
      NodeValidationState::Valid) {
    _width = std::max(_width, validationWidth());
    _height += validationHeight() + _spacing;
  }
}

void NodeGeometry::recalculateSize(QFont const &font) const {
  QFontMetrics fontMetrics(font);
  QFont boldFont = font;

  boldFont.setBold(true);

  QFontMetrics boldFontMetrics(boldFont);

  if (_boldFontMetrics != boldFontMetrics) {
    _fontMetrics = fontMetrics;
    _boldFontMetrics = boldFontMetrics;

    recalculateSize();
  }
}

QPointF NodeGeometry::portScenePosition(PortIndex index, PortType portType,
                                        QTransform const &t) const {
  auto const &nodeStyle = StyleCollection::nodeStyle();

  unsigned int step = _entryHeight + _spacing;

  QPointF result;

  double totalHeight = 0.0;

  totalHeight += captionHeight();

  auto indexes = _obj.index().model()->nodePortIndexes(_obj.index(), portType);

  if (auto found = std::find(std::begin(indexes), std::end(indexes), index);
      found != std::end(indexes)) {
    totalHeight += step * std::distance(std::begin(indexes), found);
  } else {
    GET_INFO();
  }

  totalHeight += step / 2.0;

  switch (portType) {
  case PortType::Out: {
    double x = _width + nodeStyle.ConnectionPointDiameter;

    result = QPointF(x, totalHeight);
    break;
  }

  case PortType::In: {
    double x = 0.0 - nodeStyle.ConnectionPointDiameter;

    result = QPointF(x, totalHeight);
    break;
  }

  default:
    break;
  }

  return t.map(result);
}

PortIndex
NodeGeometry::checkHitScenePoint(PortType portType, QPointF const scenePoint,
                                 QTransform const &sceneTransform) const {
  auto const &nodeStyle = StyleCollection::nodeStyle();

  PortIndex result = INVALID;

  if (portType == PortType::None)
    return result;

  double const tolerance = 2.0 * nodeStyle.ConnectionPointDiameter;

  auto items = _nodeIndex.model()->nodePortIndexes(_nodeIndex, portType);

  for (auto i : items) {
    auto pp = portScenePosition(i, portType, sceneTransform);

    QPointF p = pp - scenePoint;
    auto distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance) {
      result = PortIndex(i);
      break;
    }
  }

  return result;
}

QRect NodeGeometry::resizeRect() const {
  unsigned int rectSize = 7;

  return QRect(_width - rectSize, _height - rectSize, rectSize, rectSize);
}

QPointF NodeGeometry::widgetPosition() const {
  if (auto w = _nodeIndex.model()->nodeWidget(_nodeIndex)) {
    if (_nodeIndex.model()->nodeValidationState(_nodeIndex) !=
        NodeValidationState::Valid) {
      return QPointF(_spacing + portWidth(PortType::In),
                     (captionHeight() + _height - validationHeight() -
                      _spacing - w->height()) /
                         2.0);
    }

    return QPointF(_spacing + portWidth(PortType::In),
                   (captionHeight() + _height - w->height()) / 2.0);
  }

  return QPointF();
}

unsigned int NodeGeometry::captionHeight() const {

  QString name = _nodeIndex.model()->nodeCaption(_nodeIndex);

  return _boldFontMetrics.boundingRect(name).height();
}

unsigned int NodeGeometry::captionWidth() const {

  QString name = _nodeIndex.model()->nodeCaption(_nodeIndex);

  return _boldFontMetrics.boundingRect(name).width();
}

unsigned int NodeGeometry::validationHeight() const {
  QString msg = _nodeIndex.model()->nodeValidationMessage(_nodeIndex);

  return _boldFontMetrics.boundingRect(msg).height();
}

unsigned int NodeGeometry::validationWidth() const {
  QString msg = _nodeIndex.model()->nodeValidationMessage(_nodeIndex);

  return _boldFontMetrics.boundingRect(msg).width();
}

QPointF NodeGeometry::calculateNodePositionBetweenNodePorts(
    PortIndex targetPortIndex, PortType targetPort,
    const NodeGraphicsObject &targetNode, PortIndex sourcePortIndex,
    PortType sourcePort, const NodeGraphicsObject &sourceNode,
    const NodeGeometry &newNodeGeom) {
  // Calculating the nodes position in the scene. It'll be positioned half way
  // between the two ports that it "connects". The first line calculates the
  // halfway point between the ports (node position + port position on the node
  // for both nodes averaged). The second line offsets this coordinate with the
  // size of the new node, so that the new nodes center falls on the originally
  // calculated coordinate, instead of it's upper left corner.
  auto converterNodePos =
      (sourceNode.pos() +
       sourceNode.geometry().portScenePosition(sourcePortIndex, sourcePort) +
       targetNode.pos() +
       targetNode.geometry().portScenePosition(targetPortIndex, targetPort)) /
      2.0f;
  converterNodePos.setX(converterNodePos.x() - newNodeGeom.width() / 2.0f);
  converterNodePos.setY(converterNodePos.y() - newNodeGeom.height() / 2.0f);
  return converterNodePos;
}

unsigned int NodeGeometry::portWidth(PortType portType) const {
  unsigned width = 0;

  for (auto &i : _nodeIndex.model()->nodePortIndexes(_nodeIndex, portType)) {
    QString name = _nodeIndex.model()->nodePortCaption(_nodeIndex, i, portType);

    if (name.isEmpty()) {
      name = _nodeIndex.model()->nodePortDataType(_nodeIndex, i, portType).name;
    }

    width = std::max(unsigned(_fontMetrics.width(name)), width);
  }

  return width;
}

} // namespace QtNodes
