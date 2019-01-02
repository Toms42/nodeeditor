// main.cpp

#include "ReceiverNode.hpp"
#include "SenderNode.hpp"
#include "nodes/DataFlowModel.hpp"
#include "nodes/DataFlowScene.hpp"
#include "nodes/DataModelRegistry.hpp"
#include "nodes/FlowView.hpp"
#include <QApplication>
#include <QMenuBar>
#include <QVBoxLayout>

int main(int argc, char *argv[]) {
  ::QApplication app(argc, argv);

  auto dmRegistry = std::make_shared<QtNodes::DataModelRegistry>();
  dmRegistry->registerModel<SenderNode>();
  dmRegistry->registerModel<ReceiverNode>();

  QtNodes::DataFlowModel model(dmRegistry);
  QtNodes::DataFlowScene scene(&model);
  auto                   view = new QtNodes::FlowView{&scene};

  auto menuBar    = new QMenuBar;
  auto menu       = menuBar->addMenu("menu");
  auto saveAction = menu->addAction("Save");
  auto loadAction = menu->addAction("Load");

  QObject::connect(
      saveAction, &QAction::triggered, &scene, &QtNodes::DataFlowScene::save);
  QObject::connect(
      loadAction, &QAction::triggered, &scene, &QtNodes::DataFlowScene::load);

  QWidget mainWindow;
  auto    layout = new QVBoxLayout;
  layout->addWidget(menuBar);
  layout->addWidget(view);

  mainWindow.setLayout(layout);

  mainWindow.setWindowTitle("My custom sample");
  mainWindow.show();

  return ::QApplication::exec();
}
