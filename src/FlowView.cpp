#include "FlowView.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "DataModelRegistry.hpp"
#include "FlowScene.hpp"
#include "Node.hpp"
#include "NodeComposite.hpp"
#include "NodeGraphicsFrame.hpp"
#include "StyleCollection.hpp"
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtOpenGL>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QMenu>
#include <QtWidgets>
#include <cmath>
#include <iostream>

using QtNodes::FlowScene;
using QtNodes::FlowView;

FlowView::FlowView(QWidget *parent)
    : QGraphicsView(parent)
    , _clearSelectionAction(Q_NULLPTR)
    , _deleteSelectionAction(Q_NULLPTR)
    , _scene(Q_NULLPTR) {
  setDragMode(QGraphicsView::ScrollHandDrag);
  setRenderHint(QPainter::Antialiasing);

  auto const &flowViewStyle = StyleCollection::flowViewStyle();

  setBackgroundBrush(flowViewStyle.BackgroundColor);

  // setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  // setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  setCacheMode(QGraphicsView::CacheBackground);

  // setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
}

FlowView::FlowView(FlowScene *scene, QWidget *parent)
    : FlowView(parent) {
  setScene(scene);
}

QAction *FlowView::clearSelectionAction() const {
  return _clearSelectionAction;
}

QAction *FlowView::deleteSelectionAction() const {
  return _deleteSelectionAction;
}

void FlowView::setScene(FlowScene *scene) {
  _scene = scene;
  QGraphicsView::setScene(_scene);

  // setup actions
  delete _clearSelectionAction;
  _clearSelectionAction = new QAction(QStringLiteral("Clear Selection"), this);
  _clearSelectionAction->setShortcut(Qt::Key_Escape);
  connect(_clearSelectionAction,
          &QAction::triggered,
          _scene,
          &QGraphicsScene::clearSelection);
  addAction(_clearSelectionAction);

  delete _deleteSelectionAction;
  _deleteSelectionAction =
      new QAction(QStringLiteral("Delete Selection"), this);
  _deleteSelectionAction->setShortcut(Qt::Key_Delete);
  connect(_deleteSelectionAction,
          &QAction::triggered,
          this,
          &FlowView::deleteSelectedNodes);
  addAction(_deleteSelectionAction);
}

void FlowView::contextMenuEvent(QContextMenuEvent *event) {
  QGraphicsView::contextMenuEvent(event);
}

void FlowView::wheelEvent(QWheelEvent *event) {
  QPoint delta = event->angleDelta();

  if (delta.y() == 0) {
    event->ignore();
    return;
  }

  double const d = delta.y() / std::abs(delta.y());

  if (d > 0.0) {
    scaleUp();
  } else {
    scaleDown();
  }
}

void FlowView::scaleUp() {
  double const step   = 1.2;
  double const factor = std::pow(step, 1.0);

  QTransform t = transform();

  if (t.m11() > 2.0) {
    return;
  }

  scale(factor, factor);
}

void FlowView::scaleDown() {
  double const step   = 1.2;
  double const factor = std::pow(step, -1.0);

  scale(factor, factor);
}

void FlowView::deleteSelectedNodes() {
  // Delete the selected connections first, ensuring that they won't be
  // automatically deleted when selected nodes are deleted (deleting a node
  // deletes some connections as well)
  for (QGraphicsItem *item : _scene->selectedItems()) {
    if (item->type() == ConnectionGraphicsObject::Connection) {
      if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject *>(item)) {
        // does't matter if it works or doesn't, at least we tried
        scene()->model()->removeConnection(c->node(PortType::Out),
                                           c->portIndex(PortType::Out),
                                           c->node(PortType::In),
                                           c->portIndex(PortType::In));
      }
    }
  }

  // Delete the nodes; this will delete many of the connections.
  // Selected connections were already deleted prior to this loop, otherwise
  // qgraphicsitem_cast<NodeGraphicsObject*>(item) could be a use-after-free
  // when a selected connection is deleted by deleting the node.
  for (QGraphicsItem *item : _scene->selectedItems()) {
    if (auto n = qgraphicsitem_cast<NodeComposite *>(item)) {
      auto index = n->nodeIndex();

      scene()->model()->removeNodeWithConnections(index);
    }
  }
}

void FlowView::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Shift:
    setDragMode(QGraphicsView::RubberBandDrag);
    break;

  default:
    break;
  }

  QGraphicsView::keyPressEvent(event);
}

void FlowView::keyReleaseEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Shift:
    setDragMode(QGraphicsView::ScrollHandDrag);
    break;

  default:
    break;
  }
  QGraphicsView::keyReleaseEvent(event);
}

void FlowView::mousePressEvent(QMouseEvent *event) {
  QGraphicsView::mousePressEvent(event);
  if (event->button() == Qt::LeftButton) {
    _clickPos = mapToScene(event->pos());
  }
}

void FlowView::mouseMoveEvent(QMouseEvent *event) {
  QGraphicsView::mouseMoveEvent(event);
  if (scene()->mouseGrabberItem() == nullptr &&
      event->buttons() == Qt::LeftButton) {
    // Make sure shift is not being pressed
    if ((event->modifiers() & Qt::ShiftModifier) == 0) {
      QPointF difference = _clickPos - mapToScene(event->pos());
      setSceneRect(sceneRect().translated(difference.x(), difference.y()));
    }
  }
}

void FlowView::drawBackground(QPainter *painter, const QRectF &r) {
  QGraphicsView::drawBackground(painter, r);

  auto drawGrid = [&](double gridStep) {
    QRect   windowRect = rect();
    QPointF tl         = mapToScene(windowRect.topLeft());
    QPointF br         = mapToScene(windowRect.bottomRight());

    double left   = std::floor(tl.x() / gridStep - 0.5);
    double right  = std::floor(br.x() / gridStep + 1.0);
    double bottom = std::floor(tl.y() / gridStep - 0.5);
    double top    = std::floor(br.y() / gridStep + 1.0);

    // vertical lines
    for (int xi = int(left); xi <= int(right); ++xi) {
      QLineF line(
          xi * gridStep, bottom * gridStep, xi * gridStep, top * gridStep);

      painter->drawLine(line);
    }

    // horizontal lines
    for (int yi = int(bottom); yi <= int(top); ++yi) {
      QLineF line(
          left * gridStep, yi * gridStep, right * gridStep, yi * gridStep);
      painter->drawLine(line);
    }
  };

  auto const &flowViewStyle = StyleCollection::flowViewStyle();

  QBrush bBrush = backgroundBrush();

  QPen pfine(flowViewStyle.FineGridColor, 1.0);

  painter->setPen(pfine);
  drawGrid(15);

  QPen p(flowViewStyle.CoarseGridColor, 1.0);

  painter->setPen(p);
  drawGrid(150);
}

void FlowView::showEvent(QShowEvent *event) {
  _scene->setSceneRect(this->rect());
  QGraphicsView::showEvent(event);
}

FlowScene *FlowView::scene() {
  return _scene;
}

void FlowView::resizeEvent(QResizeEvent *event) {
  _scene->setSceneRect(QRect{QPoint{}, event->size()});
  QGraphicsView::resizeEvent(event);
}
