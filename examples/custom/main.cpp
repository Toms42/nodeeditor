// main.cpp

#include "ReceiverNode.hpp"
#include "SenderNode.hpp"
#include "nodes/DataFlowModel.hpp"
#include "nodes/DataFlowScene.hpp"
#include "nodes/DataModelRegistry.hpp"
#include "nodes/FlowView.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
  ::QApplication app(argc, argv);

  auto dmRegistry = std::make_shared<QtNodes::DataModelRegistry>();
  dmRegistry->registerModel<SenderNode>();
  dmRegistry->registerModel<ReceiverNode>();

  QtNodes::DataFlowModel model(dmRegistry);
  QtNodes::DataFlowScene scene(&model);

  QtNodes::FlowView view{&scene};

  view.setWindowTitle("My custom sample");
  view.show();

  return ::QApplication::exec();
}
