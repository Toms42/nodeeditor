#include "ImageLoaderModel.hpp"
#include "ImageShowModel.hpp"
#include <QtWidgets/QApplication>
#include <nodes/DataFlowScene.hpp>
#include <nodes/FlowView.hpp>
#include <nodes/NodeData.hpp>

using QtNodes::DataFlowScene;
using QtNodes::DataModelRegistry;
using QtNodes::FlowView;

static std::shared_ptr<DataModelRegistry> registerDataModels() {
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<ImageShowModel>();

  ret->registerModel<ImageLoaderModel>();

  return ret;
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  DataFlowScene scene(registerDataModels());

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return QApplication::exec();
}
