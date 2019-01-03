// NodeGraphicsFrame.hpp

#pragma once

#include "FlowScene.hpp"
#include "FrameGeometry.hpp"
#include "NodeComposite.hpp"
#include "NodeIndex.hpp"
#include "NodeState.hpp"

namespace QtNodes {
class NodeGraphicsFrame final : public NodeComposite {
  Q_OBJECT

public:
  NodeGraphicsFrame(FlowScene &scene, const NodeIndex &nodeIndex);

  ~NodeGraphicsFrame() override;

  int type() const override;

  bool interractWith(NodeComposite *obj) override;

  void unloadAllChilds();

  CompositeGeometry &      geometry() override;
  const CompositeGeometry &geometry() const override;

protected:
  void paint(QPainter *                      painter,
             const QStyleOptionGraphicsItem *option,
             QWidget *                       widget = nullptr) override;

  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                      const QVariant &                  value) override;

private:
  FrameGeometry geometry_;
};
}; // namespace QtNodes
