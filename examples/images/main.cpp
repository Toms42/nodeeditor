#include "ImageLoaderModel.hpp"
#include "ImageShowModel.hpp"
#include "nodes/DataFlowModel.hpp"
#include <QtWidgets/QApplication>
#include <nodes/DataFlowScene.hpp>
#include <nodes/FlowView.hpp>

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

  QtNodes::DataFlowModel model(registerDataModels());
  DataFlowScene          scene(&model);

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return QApplication::exec();
}
