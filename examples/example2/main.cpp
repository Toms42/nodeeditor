#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <QtWidgets/QApplication>

#include <nodes/DataModelRegistry>

#include "TextSourceDataModel.hpp"
#include "TextDisplayDataModel.hpp"


static std::unique_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_unique<DataModelRegistry>();
  ret->registerModel(std::make_unique<TextSourceDataModel>());

  ret->registerModel(std::make_unique<TextSourceDataModel>());

  return ret;
}

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  FlowScene scene(registerDataModels());

  FlowView view(&scene);

  view.setWindowTitle("Node-based flow editor");
  view.resize(800, 600);
  view.show();

  return app.exec();
}
