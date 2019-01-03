#include "models.hpp"
#include <QtWidgets/QApplication>
#include <nodes/ConnectionStyle.hpp>
#include <nodes/DataFlowModel.hpp>
#include <nodes/DataFlowScene.hpp>
#include <nodes/DataModelRegistry.hpp>
#include <nodes/FlowView.hpp>
#include <nodes/NodeData.hpp>

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowScene;
using QtNodes::DataModelRegistry;
using QtNodes::FlowView;

static std::shared_ptr<DataModelRegistry> registerDataModels() {
  auto ret = std::make_shared<DataModelRegistry>();

  ret->registerModel<NaiveDataModel>();

  /*
     We could have more models registered.
     All of them become items in the context meny of the scene.

     ret->registerModel<AnotherDataModel>();
     ret->registerModel<OneMoreDataModel>();

   */

  return ret;
}

static void setStyle() {
  ConnectionStyle::setConnectionStyle(
      R"(
  {
    "ConnectionStyle": {
      "UseDataDefinedColors": true
    }
  }
  )");
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  setStyle();

  QtNodes::DataFlowModel model{registerDataModels()};
  DataFlowScene          scene(&model);

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
