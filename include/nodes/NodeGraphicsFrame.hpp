// NodeGraphicsFrame.hpp

#pragma once

#include "FlowScene.hpp"
#include "NodeComposite.hpp"
#include "NodeGeometry.hpp"
#include "NodeIndex.hpp"
#include "NodeState.hpp"

namespace QtNodes {
class NodeGraphicsFrame final : public NodeComposite {
  Q_OBJECT

public:
  NodeGraphicsFrame(FlowScene &scene, const NodeIndex &nodeIndex);

  ~NodeGraphicsFrame() override;

  int type() const override;

  bool canBeParent() const override;

  /**\brief void method
   */
  void reactToPossibleConnection(PortType       portType,
                                 NodeDataType   dataType,
                                 const QPointF &scenePoint) override;

  /**\brief void method
   */
  void resetReactionToConnection() override;
};
}; // namespace QtNodes
