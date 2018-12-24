#include <nodes/DataFlowScene.hpp>
#include <nodes/FlowView.hpp>
#include <nodes/NodeData.hpp>

#include <QtWidgets/QApplication>

#include <nodes/DataModelRegistry.hpp>

#include "TextDisplayDataModel.hpp"
#include "TextSourceDataModel.hpp"

using QtNodes::DataFlowScene;
using QtNodes::DataModelRegistry;
using QtNodes::FlowView;

static std::shared_ptr<DataModelRegistry> registerDataModels() {
  auto ret = std::make_shared<DataModelRegistry>();

  ret->registerModel<TextSourceDataModel>();

  ret->registerModel<TextDisplayDataModel>();

  return ret;
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  DataFlowScene scene(registerDataModels());

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
