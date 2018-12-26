#include "models.hpp"
#include <nodes/DataFlowModel>
#include <nodes/Node>

using QtNodes::DataFlowModel;
using QtNodes::DataModelRegistry;

static std::shared_ptr<DataModelRegistry> registerDataModels() {
  auto ret = std::make_shared<DataModelRegistry>();

  ret->registerModel<Source>();
  ret->registerModel<Sink>();

  return ret;
}

int main() {
  DataFlowModel model(registerDataModels());

  auto source = model.nodeIndex(model.addNode("Source", {0, 0}));
  auto sink   = model.nodeIndex(model.addNode("Sink", {10, 0}));

  model.addConnection(source, 0, sink, 0);

  Q_ASSERT(reinterpret_cast<Sink *>(model.nodes()[sink.id()]->nodeDataModel())
               ->data == "Hello World!");
}
