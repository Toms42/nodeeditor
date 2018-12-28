// FrameGeometry.hpp

#pragma once

#include "CompositeGeometry.hpp"

namespace QtNodes {
class FrameGeometry final : public CompositeGeometry {
public:
  FrameGeometry(NodeComposite &obj);
  void recalculateSize() const override;

private:
};
}; // namespace QtNodes
