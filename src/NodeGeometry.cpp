#include "NodeGeometry.hpp"
#include "Node.hpp"
#include "NodeComposite.hpp"
#include "NodeIndex.hpp"
#include "PortType.hpp"
#include "StyleCollection.hpp"
#include "checker.hpp"
#include <QWidget>
#include <cmath>
#include <iostream>

namespace QtNodes {

NodeGeometry::NodeGeometry(NodeComposite &obj)
    : CompositeGeometry{obj}
    , _inputPortWidth(70)
    , _outputPortWidth(70) {}

unsigned int NodeGeometry::nSources() const {
  return reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
      ->nodePortCount(PortType::Out);
}

unsigned int NodeGeometry::nSinks() const {
  return reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
      ->nodePortCount(PortType::In);
}

void NodeGeometry::recalculateSize() const {
  prepareGeometryChange();

  auto bWidth  = width();
  auto bHeight = height();

  entryHeight_ = fontMetrics_.height();

  {
    unsigned int maxNumOfEntries = std::max(nSinks(), nSources());
    unsigned int step            = entryHeight_ + spacing();
    height_                      = step * maxNumOfEntries;
  }

  QWidget *w =
      reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
          ->nodeWidget();
  if (w) {
    height_ = std::max(height_, static_cast<unsigned>(w->height()));
    // if port was be dynamicly added, then current _height will be greather
    // then widget height, so it have to changed
    if (height_ > static_cast<unsigned>(w->height())) {
      w->setFixedHeight(height_);
    }
  }

  height_ += captionHeight();

  _inputPortWidth  = portWidth(PortType::In);
  _outputPortWidth = portWidth(PortType::Out);

  width_ = _inputPortWidth + _outputPortWidth + 2 * spacing();

  if (w) {
    width_ += w->width();
  }

  width_ = std::max(width(), captionWidth());

  if (reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
          ->nodeValidationState() != NodeValidationState::Valid) {
    width_ = std::max(width(), validationWidth());
    height_ += validationHeight() + spacing();
  }

  // TODO if size of object change we touch position for moveconnections
  if (bWidth != width() || bHeight != height()) {
    auto pos = obj_.pos();
    obj_.setPos({0, 0});
    obj_.setPos(pos);
  }
}

QPointF NodeGeometry::portScenePosition(PortIndex         index,
                                        PortType          portType,
                                        QTransform const &t) const {
  auto const &nodeStyle = StyleCollection::nodeStyle();

  unsigned int step = entryHeight() + spacing();

  QPointF result;

  double totalHeight = 0.0;

  totalHeight += captionHeight();

  auto indexes =
      reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
          ->nodePortIndexes(portType);

  if (auto found = std::find(std::begin(indexes), std::end(indexes), index);
      found != std::end(indexes)) {
    totalHeight += step * std::distance(std::begin(indexes), found);
  } else {
    GET_INFO();
  }

  totalHeight += step / 2.0;

  switch (portType) {
  case PortType::Out: {
    double x = width() + nodeStyle.ConnectionPointDiameter;

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
NodeGeometry::checkHitScenePoint(PortType          portType,
                                 QPointF const     scenePoint,
                                 QTransform const &sceneTransform) const {
  auto const &nodeStyle = StyleCollection::nodeStyle();

  PortIndex result = INVALID;

  if (portType == PortType::None) {
    return result;
  }

  double const tolerance = 2.0 * nodeStyle.ConnectionPointDiameter;

  auto items =
      reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
          ->nodePortIndexes(portType);

  for (auto i : items) {
    auto pp = portScenePosition(i, portType, sceneTransform);

    QPointF p        = pp - scenePoint;
    auto    distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance) {
      result = PortIndex(i);
      break;
    }
  }

  return result;
}

QPointF NodeGeometry::widgetPosition() const {
  if (auto *w =
          reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
              ->nodeWidget()) {
    if (reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
            ->nodeValidationState() != NodeValidationState::Valid) {
      return QPointF(spacing() + portWidth(PortType::In),
                     (captionHeight() + height() - validationHeight() -
                      spacing() - w->height()) /
                         2.0);
    }

    return QPointF(spacing() + portWidth(PortType::In),
                   (captionHeight() + height() - w->height()) / 2.0);
  }

  return QPointF();
}

unsigned int NodeGeometry::captionHeight() const {
  QString name =
      reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
          ->nodeCaption();

  return boldFontMetrics_.boundingRect(name).height();
}

unsigned int NodeGeometry::captionWidth() const {
  QString name =
      reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
          ->nodeCaption();

  return boldFontMetrics_.boundingRect(name).width();
}

unsigned int NodeGeometry::validationHeight() const {
  QString msg =
      reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
          ->nodeValidationMessage();

  return boldFontMetrics_.boundingRect(msg).height();
}

unsigned int NodeGeometry::validationWidth() const {
  QString msg =
      reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
          ->nodeValidationMessage();

  return boldFontMetrics_.boundingRect(msg).width();
}

QPointF NodeGeometry::calculateNodePositionBetweenNodePorts(
    PortIndex            targetPortIndex,
    PortType             targetPort,
    const NodeComposite &targetNode,
    PortIndex            sourcePortIndex,
    PortType             sourcePort,
    const NodeComposite &sourceNode,
    const NodeGeometry & newNodeGeom) {
  Q_UNUSED(targetPortIndex);
  Q_UNUSED(targetPort);
  Q_UNUSED(targetNode);
  Q_UNUSED(sourcePortIndex);
  Q_UNUSED(sourcePort);
  Q_UNUSED(sourceNode);
  Q_UNUSED(newNodeGeom);
  // Calculating the nodes position in the scene. It'll be positioned half way
  // between the two ports that it "connects". The first line calculates the
  // halfway point between the ports (node position + port position on the node
  // for both nodes averaged). The second line offsets this coordinate with the
  // size of the new node, so that the new nodes center falls on the originally
  // calculated coordinate, instead of it's upper left corner.

  // TODO I comment this, because I don't see where it uses
  // auto converterNodePos =
  //    (sourceNode.pos() +
  //     sourceNode.geometry().portScenePosition(sourcePortIndex, sourcePort) +
  //     targetNode.pos() +
  //     targetNode.geometry().portScenePosition(targetPortIndex, targetPort)) /
  //    2.0f;
  // converterNodePos.setX(converterNodePos.x() - newNodeGeom.width() / 2.0f);
  // converterNodePos.setY(converterNodePos.y() - newNodeGeom.height() / 2.0f);
  // return converterNodePos;

  return QPointF{};
}

unsigned int NodeGeometry::portWidth(PortType portType) const {
  unsigned width = 0;

  for (auto &i :
       reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
           ->nodePortIndexes(portType)) {
    QString name =
        reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
            ->nodePortCaption(portType, i);

    if (name.isEmpty()) {
      name = reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
                 ->nodePortDataType(portType, i)
                 .name;
    }

    width = std::max(unsigned(fontMetrics_.width(name)), width);
  }

  return width;
}

void NodeGeometry::setWidth(unsigned width) {
  int diff = static_cast<int>(width) - static_cast<int>(this->width());

  auto w = reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
               ->nodeWidget();

  if (w && w->width() + diff >= w->minimumSizeHint().width()) {
    w->setFixedWidth(w->width() + diff);
  }

  CompositeGeometry::setWidth(width);
}

void NodeGeometry::setHeight(unsigned height) {
  int diff = static_cast<int>(height) - static_cast<int>(this->height());

  auto w = reinterpret_cast<class Node *>(obj_.nodeIndex().internalPointer())
               ->nodeWidget();

  if (w && w->height() + diff >= w->minimumSizeHint().height()) {
    w->setFixedHeight(w->height() + diff);
  }

  CompositeGeometry::setHeight(height);
}

} // namespace QtNodes
