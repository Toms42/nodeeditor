#include "AdditionModel.hpp"
#include "Converters.hpp"
#include "DivisionModel.hpp"
#include "ModuloModel.hpp"
#include "MultiplicationModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "NumberSourceDataModel.hpp"
#include "SubtractionModel.hpp"
#include "nodes/DataFlowModel.hpp"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include <nodes/ConnectionStyle.hpp>
#include <nodes/DataFlowScene.hpp>
#include <nodes/DataModelRegistry.hpp>
#include <nodes/FlowView.hpp>
#include <nodes/NodeData.hpp>
#include <nodes/TypeConverter.hpp>

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowScene;
using QtNodes::DataModelRegistry;
using QtNodes::FlowView;
using QtNodes::TypeConverter;
using QtNodes::TypeConverterId;

static std::shared_ptr<DataModelRegistry> registerDataModels() {
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<NumberSourceDataModel>("Sources");

  ret->registerModel<NumberDisplayDataModel>("Displays");

  ret->registerModel<AdditionModel>("Operators");

  ret->registerModel<SubtractionModel>("Operators");

  ret->registerModel<MultiplicationModel>("Operators");

  ret->registerModel<DivisionModel>("Operators");

  ret->registerModel<ModuloModel>("Operators");

  ret->registerTypeConverter(
      std::make_pair(DecimalData().type(), IntegerData().type()),
      TypeConverter{DecimalToIntegerConverter()});

  ret->registerTypeConverter(
      std::make_pair(IntegerData().type(), DecimalData().type()),
      TypeConverter{IntegerToDecimalConverter()});

  return ret;
}

static void setStyle() {
  ConnectionStyle::setConnectionStyle(
      R"(
  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "black",
      "SelectedColor": "gray",
      "SelectedHaloColor": "deepskyblue",
      "HoveredColor": "deepskyblue",

      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,

      "UseDataDefinedColors": true
    }
  }
  )");
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  setStyle();

  QWidget mainWidget;

  auto menuBar    = new QMenuBar(&mainWidget);
  auto menu       = menuBar->addMenu("menu");
  auto saveAction = menu->addAction("Save..");
  auto loadAction = menu->addAction("Load..");

  QVBoxLayout *l = new QVBoxLayout(&mainWidget);

  l->addWidget(menuBar);
  auto model = new QtNodes::DataFlowModel(registerDataModels(), &mainWidget);
  auto scene = new DataFlowScene(model, &mainWidget);
  l->addWidget(new FlowView(scene));
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  QObject::connect(
      saveAction, &QAction::triggered, scene, &DataFlowScene::save);

  QObject::connect(
      loadAction, &QAction::triggered, scene, &DataFlowScene::load);

  mainWidget.setWindowTitle("Dataflow tools: simplest calculator");
  mainWidget.resize(800, 600);
  mainWidget.show();

  // FlowView newViewForOldScene(scene);
  // newViewForOldScene.show();

  // auto     newScene = new DataFlowScene(model);
  // FlowView newViewOldModel(newScene);
  // newViewOldModel.show();

  return app.exec();
}
