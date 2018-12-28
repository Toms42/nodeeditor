// FramePainter.cpp

#include "FramePainter.hpp"
#include "FlowSceneModel.hpp"
#include "NodeComposite.hpp"
#include <QPainter>

namespace QtNodes {
void FramePainter::paint(QPainter *painter, const NodeComposite &obj) {
  drawNodeRect(painter, obj);
  drawResizeRect(painter, obj);
}

void FramePainter::drawNodeRect(QPainter *painter, const NodeComposite &obj) {
  // auto &           model     = *obj.nodeIndex().model();
  // NodeStyle const &nodeStyle = model.nodeStyle(obj.nodeIndex());
  auto &geom = obj.geometry();

  // auto color = obj.isSelected() ? nodeStyle.SelectedBoundaryColor
  //                              : nodeStyle.NormalBoundaryColor;

  // if (geom.hovered()) {
  //  QPen p(color, nodeStyle.HoveredPenWidth);
  //  painter->setPen(p);
  //} else {
  //  QPen p(color, nodeStyle.PenWidth);
  //  painter->setPen(p);
  //}

  QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(2.0, geom.height()));

  // gradient.setColorAt(0.0, nodeStyle.GradientColor0);
  // gradient.setColorAt(0.03, nodeStyle.GradientColor1);
  // gradient.setColorAt(0.97, nodeStyle.GradientColor2);
  // gradient.setColorAt(1.0, nodeStyle.GradientColor3);

  painter->setBrush(gradient);

  // float diam = nodeStyle.ConnectionPointDiameter;

  QRectF boundary(0, 0, geom.width(), geom.height());

  // double const radius = 3.0;

  painter->drawRoundedRect(boundary, 5, 5);
}

void FramePainter::drawResizeRect(QPainter *painter, const NodeComposite &obj) {
  // FlowSceneModel const &model = *obj.nodeIndex().model();

  painter->setBrush(Qt::gray);

  painter->drawEllipse(obj.geometry().resizeRect());
}
} // namespace QtNodes
