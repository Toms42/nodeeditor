#pragma once

#include "CompositeGeometry.hpp"
#include "Export.hpp"
#include "NodeIndex.hpp"
#include "PortType.hpp"
#include "memory.hpp"

namespace QtNodes {

class NODE_EDITOR_PUBLIC NodeGeometry final : public CompositeGeometry {
public:
  NodeGeometry(NodeComposite &obj);

  unsigned int nSources() const;

  unsigned int nSinks() const;

  /// Updates size unconditionally
  void recalculateSize() const override;

  // TODO removed default QTransform()
  QPointF portScenePosition(PortIndex         index,
                            PortType          portType,
                            QTransform const &t = QTransform()) const;

  PortIndex checkHitScenePoint(PortType          portType,
                               QPointF           point,
                               QTransform const &t = QTransform()) const;

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

private:
  unsigned int captionHeight() const;

  unsigned int captionWidth() const;

  unsigned int portWidth(PortType portType) const;

private:
  mutable unsigned int _inputPortWidth;
  mutable unsigned int _outputPortWidth;
};
} // namespace QtNodes
