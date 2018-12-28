// FrameGeometry.cpp

#include "FrameGeometry.hpp"

namespace QtNodes {
FrameGeometry::FrameGeometry(NodeComposite &obj)
    : CompositeGeometry{obj} {}

void FrameGeometry::recalculateSize() const {}
} // namespace QtNodes
