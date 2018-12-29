#include "NodePainter.hpp"
#include "Node.hpp"
#include "NodeComposite.hpp"
#include "NodeGeometry.hpp"
#include "NodeIndex.hpp"
#include "NodePainterDelegate.hpp"
#include "NodeState.hpp"
#include "PortType.hpp"
#include "StyleCollection.hpp"
#include "checker.hpp"
#include <QtCore/QMargins>
#include <cmath>
#include <iostream>

using QtNodes::ConnectionPolicy;
using QtNodes::NodeGeometry;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodePainter;
using QtNodes::NodeState;

void NodePainter::paint(QPainter *           painter,
                        NodeComposite const &graphicsObject) {
  auto &geom = graphicsObject.geometry();

  geom.recalculateSize(painter->font());

  //--------------------------------------------

  drawNodeRect(painter, graphicsObject);

  drawConnectionPoints(painter, graphicsObject);

  drawFilledConnectionPoints(painter, graphicsObject);

  drawModelName(painter, graphicsObject);

  drawEntryLabels(painter, graphicsObject);

  drawValidationRect(painter, graphicsObject);

  drawResizeRect(painter, graphicsObject);

  /// call custom painter
  if (auto painterDelegate = reinterpret_cast<class Node *>(
                                 graphicsObject.nodeIndex().internalPointer())
                                 ->nodePainterDelegate()) {
    painterDelegate->paint(
        painter, graphicsObject.geometry(), graphicsObject.nodeIndex());
  }
}

void NodePainter::drawNodeRect(QPainter *           painter,
                               NodeComposite const &graphicsObject) {
  auto  nodeStyle = NodeStyle{};
  auto &geom      = graphicsObject.geometry();

  auto color = graphicsObject.isSelected() ? nodeStyle.SelectedBoundaryColor
                                           : nodeStyle.NormalBoundaryColor;

  if (geom.hovered()) {
    QPen p(color, nodeStyle.HoveredPenWidth);
    painter->setPen(p);
  } else {
    QPen p(color, nodeStyle.PenWidth);
    painter->setPen(p);
  }

  QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(2.0, geom.height()));

  gradient.setColorAt(0.0, nodeStyle.GradientColor0);
  gradient.setColorAt(0.03, nodeStyle.GradientColor1);
  gradient.setColorAt(0.97, nodeStyle.GradientColor2);
  gradient.setColorAt(1.0, nodeStyle.GradientColor3);

  painter->setBrush(gradient);

  float diam = nodeStyle.ConnectionPointDiameter;

  QRectF boundary(
      -diam, -diam, 2.0 * diam + geom.width(), 2.0 * diam + geom.height());

  double const radius = 3.0;

  painter->drawRoundedRect(boundary, radius, radius);
}

void NodePainter::drawConnectionPoints(QPainter *           painter,
                                       NodeComposite const &graphicsObject) {
  auto             nodeStyle       = NodeStyle{};
  auto const &     connectionStyle = StyleCollection::connectionStyle();
  NodeState const &state           = graphicsObject.nodeState();
  // TODO check this
  try {
    auto &geom = dynamic_cast<const NodeGeometry &>(graphicsObject.geometry());

    float diameter        = nodeStyle.ConnectionPointDiameter;
    auto  reducedDiameter = diameter * 0.6;

    for (PortType portType : {PortType::Out, PortType::In}) {
      for (auto &i : state.getEntries(portType)) {
        QPointF p = geom.portScenePosition(i.first, portType);

        auto dataType = reinterpret_cast<class Node *>(
                            graphicsObject.nodeIndex().internalPointer())
                            ->nodePortDataType(portType, i.first);

        bool canConnect =
            (state.getEntries(portType).at(i.first).empty() ||
             (portType == PortType::Out &&
              reinterpret_cast<class Node *>(
                  graphicsObject.nodeIndex().internalPointer())
                      ->nodePortConnectionPolicy(PortType::Out, i.first) ==
                  ConnectionPolicy::Many));

        double r = 1.0;
        if (state.isReacting() && canConnect &&
            portType == state.reactingPortType()) {
          auto   diff            = geom.draggingPos() - p;
          double dist            = std::sqrt(QPointF::dotProduct(diff, diff));
          bool   typeConvertable = false;

          // TODO I don't know what it means
          {
            //          if (portType == PortType::In) {
            //            typeConvertable = model.getTypeConvertable(
            //                {state.reactingDataType(), dataType});
            //          } else {
            //            typeConvertable = model.getTypeConvertable(
            //                {dataType, state.reactingDataType()});
            //          }
          }

          if (state.reactingDataType().id == dataType.id || typeConvertable) {
            double const thres = 40.0;
            r                  = (dist < thres) ? (2.0 - dist / thres) : 1.0;
          } else {
            double const thres = 80.0;
            r                  = (dist < thres) ? (dist / thres) : 1.0;
          }
        }

        if (connectionStyle.useDataDefinedColors()) {
          painter->setBrush(connectionStyle.normalColor(dataType.id));
        } else {
          painter->setBrush(nodeStyle.ConnectionPointColor);
        }

        painter->drawEllipse(p, reducedDiameter * r, reducedDiameter * r);
      }
    };
  } catch (std::bad_cast &) {
    GET_INFO();
  }
}

void NodePainter::drawFilledConnectionPoints(
    QPainter *painter, NodeComposite const &graphicsObject) {
  auto             nodeStyle       = NodeStyle{};
  auto const &     connectionStyle = StyleCollection::connectionStyle();
  NodeState const &state           = graphicsObject.nodeState();
  // TODO check this
  try {
    NodeGeometry const &geom =
        dynamic_cast<const NodeGeometry &>(graphicsObject.geometry());

    auto diameter = nodeStyle.ConnectionPointDiameter;

    for (PortType portType : {PortType::Out, PortType::In}) {
      for (auto &i : state.getEntries(portType)) {
        QPointF p = geom.portScenePosition(i.first, portType);

        if (!state.getEntries(portType).at(i.first).empty()) {
          auto dataType = reinterpret_cast<class Node *>(
                              graphicsObject.nodeIndex().internalPointer())
                              ->nodePortDataType(portType, i.first);

          if (connectionStyle.useDataDefinedColors()) {
            QColor const c = connectionStyle.normalColor(dataType.id);
            painter->setPen(c);
            painter->setBrush(c);
          } else {
            painter->setPen(nodeStyle.FilledConnectionPointColor);
            painter->setBrush(nodeStyle.FilledConnectionPointColor);
          }

          painter->drawEllipse(p, diameter * 0.4, diameter * 0.4);
        }
      }
    }
  } catch (std::bad_alloc &) {
    GET_INFO();
  }
}

void NodePainter::drawModelName(QPainter *           painter,
                                NodeComposite const &graphicsObject) {
  auto  nodeStyle = NodeStyle{};
  auto &geom      = graphicsObject.geometry();

  QString const &name = reinterpret_cast<class Node *>(
                            graphicsObject.nodeIndex().internalPointer())
                            ->nodeCaption();

  if (name.isEmpty()) {
    return;
  }

  QFont f = painter->font();

  f.setBold(true);

  QFontMetrics metrics(f);
  auto         rect = metrics.boundingRect(name);
  QPointF      position((geom.width() - rect.width()) / 2.0,
                   (geom.spacing() + geom.entryHeight()) / 3.0);

  painter->setFont(f);
  painter->setPen(nodeStyle.FontColor);
  painter->drawText(position, name);

  f.setBold(false);
  painter->setFont(f);
}

void NodePainter::drawEntryLabels(QPainter *           painter,
                                  NodeComposite const &graphicsObject) {
  NodeState const &state = graphicsObject.nodeState();
  // TODO check this
  try {
    auto &geom = dynamic_cast<const NodeGeometry &>(graphicsObject.geometry());
    QFontMetrics const &metrics = painter->fontMetrics();

    for (PortType portType : {PortType::Out, PortType::In}) {
      auto nodeStyle = NodeStyle{};

      for (auto &i : state.getEntries(portType)) {
        QPointF p = geom.portScenePosition(i.first, portType);

        //    if (entries[i].empty())
        if (i.second.empty()) {
          painter->setPen(nodeStyle.FontColorFaded);
        } else {
          painter->setPen(nodeStyle.FontColor);
        }

        QString s = reinterpret_cast<class Node *>(
                        graphicsObject.nodeIndex().internalPointer())
                        ->nodePortCaption(portType, i.first);

        if (s.isEmpty()) {
          s = reinterpret_cast<class Node *>(
                  graphicsObject.nodeIndex().internalPointer())
                  ->nodePortDataType(portType, i.first)
                  .name;
        }

        auto rect = metrics.boundingRect(s);

        p.setY(p.y() + rect.height() / 4.0);

        switch (portType) {
        case PortType::In:
          p.setX(5.0);
          break;

        case PortType::Out:
          p.setX(geom.width() - 5.0 - rect.width());
          break;

        default:
          break;
        }

        painter->drawText(p, s);
      }
    }
  } catch (std::bad_alloc &) {
    GET_INFO();
  }
}

void NodePainter::drawResizeRect(QPainter *           painter,
                                 NodeComposite const &graphicsObject) {
  painter->setBrush(Qt::gray);

  painter->drawEllipse(graphicsObject.geometry().resizeRect());
}

void NodePainter::drawValidationRect(QPainter *           painter,
                                     NodeComposite const &graphicsObject) {
  auto &geom = dynamic_cast<const NodeGeometry &>(graphicsObject.geometry());

  auto modelValidationState = reinterpret_cast<class Node *>(
                                  graphicsObject.nodeIndex().internalPointer())
                                  ->nodeValidationState();

  if (modelValidationState != NodeValidationState::Valid) {
    auto nodeStyle = NodeStyle{};

    auto color = graphicsObject.isSelected() ? nodeStyle.SelectedBoundaryColor
                                             : nodeStyle.NormalBoundaryColor;

    if (geom.hovered()) {
      QPen p(color, nodeStyle.HoveredPenWidth);
      painter->setPen(p);
    } else {
      QPen p(color, nodeStyle.PenWidth);
      painter->setPen(p);
    }

    // Drawing the validation message background
    if (modelValidationState == NodeValidationState::Error) {
      painter->setBrush(nodeStyle.ErrorColor);
    } else {
      painter->setBrush(nodeStyle.WarningColor);
    }

    double const radius = 3.0;

    float diam = nodeStyle.ConnectionPointDiameter;

    QRectF boundary(-diam,
                    -diam + geom.height() - geom.validationHeight(),
                    2.0 * diam + geom.width(),
                    2.0 * diam + geom.validationHeight());

    painter->drawRoundedRect(boundary, radius, radius);

    painter->setBrush(Qt::gray);

    // Drawing the validation message itself
    QString const &errorMsg = reinterpret_cast<class Node *>(
                                  graphicsObject.nodeIndex().internalPointer())
                                  ->nodeValidationMessage();

    QFont f = painter->font();

    QFontMetrics metrics(f);

    auto rect = metrics.boundingRect(errorMsg);

    QPointF position((geom.width() - rect.width()) / 2.0,
                     geom.height() - (geom.validationHeight() - diam) / 2.0);

    painter->setFont(f);
    painter->setPen(nodeStyle.FontColor);
    painter->drawText(position, errorMsg);
  }
}
