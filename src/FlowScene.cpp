#include "FlowScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "FlowSceneModel.hpp"
#include "Node.hpp"
#include "NodeDataModel.hpp"
#include "NodeGraphicsFrame.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeIndex.hpp"
#include "checker.hpp"
#include <QGraphicsSceneContextMenuEvent>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QTreeWidget>
#include <QWidgetAction>
#include <algorithm>
#include <iostream>

using QtNodes::FlowScene;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeIndex;

namespace QtNodes {
FlowScene::FlowScene(FlowSceneModel *model, QObject *parent)
    : QGraphicsScene(parent)
    , _model(model) {
  Q_ASSERT(model != nullptr);

  connect(this, &FlowScene::addNode, model, &FlowSceneModel::addNode);

  connect(this,
          &FlowScene::removeNode,
          model,
          &FlowSceneModel::removeNodeWithConnections);

  connect(this,
          &FlowScene::removeConnection,
          model,
          &FlowSceneModel::removeConnection);

  connect(model, &FlowSceneModel::nodeRemoved, this, &FlowScene::nodeRemoved);
  connect(model, &FlowSceneModel::nodeAdded, this, &FlowScene::nodeAdded);
  // connect(model,
  //        &FlowSceneModel::nodePortUpdated,
  //        this,
  //        &FlowScene::nodePortUpdated);
  connect(model,
          &FlowSceneModel::nodeValidationUpdated,
          this,
          &FlowScene::nodeValidationUpdated);
  connect(model,
          &FlowSceneModel::connectionRemoved,
          this,
          &FlowScene::connectionRemoved);
  connect(model,
          &FlowSceneModel::connectionAdded,
          this,
          &FlowScene::connectionAdded);
  connect(model, &FlowSceneModel::updateNode, this, &FlowScene::updateNode);
  connect(model,
          &FlowSceneModel::updateConnection,
          this,
          &FlowScene::updateConnection);

  // emit node added on all the existing nodes
  for (const auto &n : model->nodeUUids()) {
    nodeAdded(n);
  }

  // add connections
  for (const auto &n : model->nodeUUids()) {
    auto id = model->nodeIndex(n);
    Q_ASSERT(id.isValid());

    for (auto &i : model->nodePortIndexes(id, PortType::Out)) {
      auto connections = model->nodePortConnections(id, i, PortType::Out);

      // validate the sanity of the model--make sure if it is marked as one
      // connection per port then there is no more than one connection
      Q_ASSERT(model->nodePortConnectionPolicy(id, i, PortType::Out) ==
                   ConnectionPolicy::Many ||
               connections.size() <= 1);

      for (const auto &conn : connections) {
        connectionAdded(id, i, conn.first, conn.second);
      }
    }
  }

  // for some reason these end up in the wrong spot, fix that
  for (const auto &n : model->nodeUUids()) {
    nodeComposite(model->nodeIndex(n))->update();
  }
}

FlowScene::~FlowScene() {
  // here we delete items for guarante that no one item not be called after
  // deleting the scene (itmes have reference to it, so they have to be deleted
  // in the destuctor, or before)
  while (!_connGraphicsObjects.empty()) {
    auto connId = _connGraphicsObjects.begin()->first;
    try {
      connectionRemoved(nodeComposites_.at(connId.lNodeID)->nodeIndex(),
                        connId.lPortID,
                        nodeComposites_.at(connId.rNodeID)->nodeIndex(),
                        connId.rPortID);
    } catch (std::out_of_range &) {
      GET_INFO();
    }
  }
  while (!nodeComposites_.empty()) {
    nodeRemoved(nodeComposites_.begin()->first);
  }
}

QtNodes::ConnectionGraphicsObject *FlowScene::temporaryConn() const {
  return _temporaryConn;
}

void FlowScene::setTemporaryConn(ConnectionGraphicsObject *obj) {
  _temporaryConn = obj;
  if (_temporaryConn) {
    addItem(_temporaryConn);
  }
}

QtNodes::NodeComposite *FlowScene::nodeComposite(const NodeIndex &index) {
  auto iter = nodeComposites_.find(index.id());
  if (iter == nodeComposites_.end()) {
    return nullptr;
  }
  return iter->second;
}

std::vector<NodeIndex> FlowScene::selectedNodes() const {
  QList<QGraphicsItem *> graphicsItems = selectedItems();

  std::vector<NodeIndex> ret;
  ret.reserve(graphicsItems.size());

  for (QGraphicsItem *item : graphicsItems) {
    auto ngo = qgraphicsitem_cast<NodeGraphicsObject *>(item);

    if (ngo != nullptr) {
      ret.push_back(ngo->nodeIndex());
    }
  }

  return ret;
}

// model slots

void FlowScene::nodeRemoved(const QUuid &id) {
  Q_ASSERT(!id.isNull());

  try {
    auto ngo = nodeComposites_.at(id);

    delete ngo;
    auto erased = nodeComposites_.erase(id);
    Q_ASSERT(erased == 1);
  } catch (std::out_of_range &) {
    GET_INFO();
  }
}

QtNodes::NodeComposite *FlowScene::createComposite(const NodeIndex &index) {
  NodeComposite *obj{};
  if (index.isValid()) {
    obj = new NodeGraphicsObject(*this, index);
  } else {
    obj = new NodeGraphicsFrame(*this, index);
  }
  addItem(obj);
  return obj;
}

void FlowScene::nodeAdded(const QUuid &newID) {
  Q_ASSERT(!newID.isNull());

  // make sure the ID doens't exist already
  Q_ASSERT(nodeComposites_.find(newID) == nodeComposites_.end());

  auto index = model()->nodeIndex(newID);

  auto ngo = createComposite(index);
  Q_ASSERT(ngo->scene() == this);

  if (!nodeComposites_.insert(std::pair(newID, ngo)).second) {
    GET_INFO();
  }

  ngo->setPos(lastPos_);
}

// TODO not work correctly!
// void FlowScene::nodePortUpdated(NodeIndex const &id) {
//  Q_ASSERT(!id.id().isNull());

//  auto thisNodeNGO = nodeComposite(id);
//  Q_ASSERT(thisNodeNGO != Q_NULLPTR);

//  // remove all the connections
//  for (auto ty : {PortType::In, PortType::Out}) {
//    for (auto &i : thisNodeNGO->nodeState().getEntries(ty)) {
//      try {
//        while (!thisNodeNGO->nodeState().getEntries(ty).at(i.first).empty()) {
//          auto conn = thisNodeNGO->nodeState().getEntries(ty).at(i.first)[0];
//          // remove it from the nodes
//          auto &otherNgo = *nodeComposite(conn->node(oppositePort(ty)));
//          otherNgo.nodeState().eraseConnection(
//              oppositePort(ty), conn->portIndex(oppositePort(ty)), *conn);

//          auto &thisNgo = *nodeComposite(conn->node(ty));
//          thisNgo.nodeState().eraseConnection(ty, conn->portIndex(ty), *conn);

//          // remove the ConnectionGraphicsObject
//          auto erased = _connGraphicsObjects.erase(conn->id());
//          conn->deleteLater();
//          Q_ASSERT(erased == 1);
//        }
//      } catch (std::out_of_range &) {
//        GET_INFO();
//      }
//    }
//  }

//  // recreate the NGO

//  // just delete it
//  delete thisNodeNGO;
//  auto erased = nodeComposites_.erase(id.id());

//  Q_ASSERT(erased == 1);

//  // create it
//  auto ngo = new NodeGraphicsObject(*this, id);
//  Q_ASSERT(ngo->scene() == this);

//  if (!nodeComposites_.insert(std::pair(id.id(), ngo)).second) {
//    GET_INFO();
//  }

//  // add the connections back
//  for (auto ty : {PortType::In, PortType::Out}) {
//    for (auto &i : model()->nodePortIndexes(id, ty)) {
//      auto connections = model()->nodePortConnections(id, i, ty);
//#ifndef QT_NO_DEBUG
//      for (auto conn : connections) {
//        auto remoteConns = model()->nodePortConnections(
//            conn.first, conn.second, oppositePort(ty));

//        // if you fail here, your connections aren't self-consistent
//        //      Q_ASSERT(std::any_of(remoteConns.begin(), remoteConns.end(),
//        //                           [&](std::pair<NodeIndex, PortIndex>
//        //                           this_conn) {
//        //                             return this_conn.first == id &&
//        //                                    (size_t)this_conn.second == i;
//        //                           }));
//      }
//#endif
//      // validate the sanity of the model--make sure if it is marked as one
//      // connection per port then there is no more than one connection
//      Q_ASSERT(model()->nodePortConnectionPolicy(id, i, ty) ==
//                   ConnectionPolicy::Many ||
//               connections.size() <= 1);

//      for (const auto &conn : connections) {
//        if (ty == PortType::Out) {
//          connectionAdded(id, i, conn.first, conn.second);
//        } else {
//          connectionAdded(conn.first, conn.second, id, i);
//        }
//      }
//    }
//  }
//}

void FlowScene::nodeValidationUpdated(NodeIndex const &id) {
  // repaint
  nodeComposite(id)->update();
}

void FlowScene::connectionRemoved(NodeIndex const &leftNode,
                                  PortIndex        leftPortID,
                                  NodeIndex const &rightNode,
                                  PortIndex        rightPortID) {
  // check the model's sanity
#ifndef QT_NO_DEBUG
// for (const auto &conn :
//     model()->nodePortConnections(leftNode, leftPortID, PortType::Out)) {
//  // if you fail here, then you're emitting connectionRemoved on a connection
//  // that is in the model
//  Q_ASSERT(conn.first != rightNode || conn.second != rightPortID);
//}
// for (const auto &conn :
//     model()->nodePortConnections(rightNode, rightPortID, PortType::In)) {
//  // if you fail here, then you're emitting connectionRemoved on a connection
//  // that is in the model
//  Q_ASSERT(conn.first != leftNode || conn.second != leftPortID);
//}
#endif

  // create a connection ID
  ConnectionID id;
  id.lNodeID = leftNode.id();
  id.rNodeID = rightNode.id();
  id.lPortID = leftPortID;
  id.rPortID = rightPortID;

  // cgo
  try {
    auto &cgo = *_connGraphicsObjects.at(id);
    // remove it from the nodes
    auto &lngo = *nodeComposite(leftNode);
    lngo.nodeState().eraseConnection(PortType::Out, leftPortID, cgo);

    auto &rngo = *nodeComposite(rightNode);
    rngo.nodeState().eraseConnection(PortType::In, rightPortID, cgo);

    // remove the ConnectionGraphicsObject
    delete &cgo;
    _connGraphicsObjects.erase(id);

    // update validation
    nodeValidationUpdated(leftNode);
    nodeValidationUpdated(rightNode);
  } catch (std::out_of_range &) {
    GET_INFO();
  }
}
void FlowScene::connectionAdded(NodeIndex const &leftNode,
                                PortIndex        leftPortID,
                                NodeIndex const &rightNode,
                                PortIndex        rightPortID) {
  // check the model's sanity
#ifndef QT_NO_DEBUG
  // Q_ASSERT(leftPortID >= 0);
  // Q_ASSERT(rightPortID >= 0);
  // if you fail here, then you're emitting connectionAdded on a portID that
  // doesn't exist
  // Q_ASSERT((size_t)leftPortID <
  //         model()->nodePortCount(leftNode, PortType::Out));
  // Q_ASSERT((size_t)rightPortID <
  //         model()->nodePortCount(rightNode, PortType::In));

  bool checkedOut = false;
  for (const auto &conn :
       model()->nodePortConnections(leftNode, leftPortID, PortType::Out)) {
    if (conn.first == rightNode && conn.second == rightPortID) {
      checkedOut = true;
      break;
    }
  }
  // if you fail here, then you're emitting connectionAdded on a connection that
  // isn't in the model
  Q_ASSERT(checkedOut);
  checkedOut = false;
  for (const auto &conn :
       model()->nodePortConnections(rightNode, rightPortID, PortType::In)) {
    if (conn.first == leftNode && conn.second == leftPortID) {
      checkedOut = true;
      break;
    }
  }
  // if you fail here, then you're emitting connectionAdded on a connection that
  // isn't in the model
  Q_ASSERT(checkedOut);
#endif

  // create the cgo
  auto cgo = new ConnectionGraphicsObject(
      leftNode, leftPortID, rightNode, rightPortID, *this);
  addItem(cgo);

  // add it to the nodes
  auto lngo = nodeComposite(leftNode);
  lngo->nodeState().setConnection(PortType::Out, leftPortID, *cgo);

  auto rngo = nodeComposite(rightNode);
  rngo->nodeState().setConnection(PortType::In, rightPortID, *cgo);

  // add the cgo to the map
  _connGraphicsObjects.insert(std::pair(cgo->id(), cgo));
}

//------------------------------------------------------------------------------

NodeComposite *locateNodeAt(QPointF           scenePoint,
                            FlowScene &       scene,
                            QTransform const &viewTransform) {
  // items under cursor
  QList<QGraphicsItem *> items = scene.items(
      scenePoint, Qt::IntersectsItemShape, Qt::DescendingOrder, viewTransform);

  //// items convertable to NodeGraphicsObject
  std::vector<QGraphicsItem *> filteredItems;

  std::copy_if(items.begin(),
               items.end(),
               std::back_inserter(filteredItems),
               [](QGraphicsItem *item) {
                 return (dynamic_cast<NodeGraphicsObject *>(item) != nullptr);
               });

  if (!filteredItems.empty()) {
    QGraphicsItem *graphicsItem = filteredItems.front();
    auto           ngo = dynamic_cast<NodeGraphicsObject *>(graphicsItem);

    return ngo;
  }

  return nullptr;
}

void FlowScene::updateNode(const NodeIndex &nodeIndex) {
  nodeComposite(nodeIndex)->update();
}

void FlowScene::updateConnection(const NodeIndex &leftNodeIndex,
                                 PortIndex        leftPortIndex,
                                 const NodeIndex &rightNodeIndex,
                                 PortIndex        rightPortIndex) {
  // create a connection ID
  ConnectionID id;
  id.lNodeID = leftNodeIndex.id();
  id.rNodeID = rightNodeIndex.id();
  id.lPortID = leftPortIndex;
  id.rPortID = rightPortIndex;

  // cgo
  CHECK_OUT_OF_RANGE(_connGraphicsObjects.at(id)->move());
}

QMenu *FlowScene::createContextMenu() {
  auto modelMenu = new QMenu;

  auto skipText = QStringLiteral("skip me");

  // Add filterbox to the context menu
  auto *txtBox = new QLineEdit(modelMenu);

  txtBox->setPlaceholderText(QStringLiteral("Filter"));
  txtBox->setClearButtonEnabled(true);

  auto *txtBoxAction = new QWidgetAction(modelMenu);
  txtBoxAction->setDefaultWidget(txtBox);

  modelMenu->addAction(txtBoxAction);

  // Add result treeview to the context menu
  auto *treeView = new QTreeWidget(modelMenu);
  treeView->header()->close();

  auto *treeViewAction = new QWidgetAction(modelMenu);
  treeViewAction->setDefaultWidget(treeView);

  modelMenu->addAction(treeViewAction);

  QMap<QString, QTreeWidgetItem *> categoryItems;
  for (auto const &modelName : model()->modelRegistry()) {
    // get the category
    auto category = model()->nodeTypeCategory(modelName);

    // see if it's already in the map
    auto iter = categoryItems.find(category);

    // add it if it doesn't exist
    if (iter == categoryItems.end()) {
      auto item = new QTreeWidgetItem(treeView);
      item->setText(0, category);
      item->setData(0, Qt::UserRole, skipText);

      iter = categoryItems.insert(category, item);
    }

    // this is the category item
    auto parent = iter.value();

    // add the item

    auto item = new QTreeWidgetItem(parent);
    item->setText(0, modelName);
    item->setData(0, Qt::UserRole, modelName);
  }

  treeView->expandAll();

  connect(treeView,
          &QTreeWidget::itemClicked,
          /*here we set by value, because referencies can be not valid*/
          [this, modelMenu, skipText](QTreeWidgetItem *item, int) {
            QString modelName = item->data(0, Qt::UserRole).toString();

            if (modelName == skipText) {
              return;
            }

            emit addNode(modelName);

            modelMenu->close();
          });

  // Setup filtering
  connect(
      /*here we set by value, because referencies can be not valid*/
      txtBox,
      &QLineEdit::textChanged,
      [categoryItems](const QString &text) {
        for (auto &topLvlItem : categoryItems) {
          for (int i = 0; i < topLvlItem->childCount(); ++i) {
            auto       child     = topLvlItem->child(i);
            auto       modelName = child->data(0, Qt::UserRole).toString();
            const bool match = (modelName.contains(text, Qt::CaseInsensitive));
            child->setHidden(!match);
          }
        }
      });

  // make sure the text box gets focus so the user doesn't have to click on it
  txtBox->setFocus();

  auto *frameAction = modelMenu->addAction("Frame");
  connect(frameAction, &QAction::triggered, this, [this, modelMenu]() {
    auto uuid = QUuid::createUuid();
    nodeAdded(uuid);
    modelMenu->close();
  });

  return modelMenu;
}

void FlowScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  lastPos_ = event->scenePos();
  if (itemAt(event->scenePos(), QTransform())) {
    QGraphicsScene::contextMenuEvent(event);
    return;
  }

  auto modelMenu = createContextMenu();

  modelMenu->exec(event->screenPos());
  event->accept();
  modelMenu->deleteLater();
}

void FlowScene::deleteSelectedNodes() {
  // Delete the selected connections first, ensuring that they won't be
  // automatically deleted when selected nodes are deleted (deleting a node
  // deletes some connections as well)
  for (QGraphicsItem *item : selectedItems()) {
    if (item->type() == ConnectionGraphicsObject::Connection) {
      if (auto c = qgraphicsitem_cast<ConnectionGraphicsObject *>(item)) {
        // does't matter if it works or doesn't, at least we tried
        emit removeConnection(c->node(PortType::Out),
                              c->portIndex(PortType::Out),
                              c->node(PortType::In),
                              c->portIndex(PortType::In));
      }
    }
  }

  // Delete the nodes; this will delete many of the connections.
  // Selected connections were already deleted prior to this loop, otherwise
  // qgraphicsitem_cast<NodeGraphicsObject*>(item) could be a use-after-free
  // when a selected connection is deleted by deleting the node.
  for (QGraphicsItem *item : selectedItems()) {
    if (auto n = qgraphicsitem_cast<NodeComposite *>(item)) {
      recursivelyRemove(n);
    }
  }
}

void FlowScene::recursivelyRemove(NodeComposite *obj) {
  // if nodeIndex is not valid - this means that this item is only in scene
  if (!obj) {
    return;
  }
  if (auto index = obj->nodeIndex(); index.isValid()) {
    emit removeNode(index.id());
  } else {
    for (auto &i : obj->childItems()) {
      recursivelyRemove(dynamic_cast<NodeComposite *>(i));
    }
    CHECK_OUT_OF_RANGE(delete nodeComposites_.at(index.id()));
    nodeComposites_.erase(index.id());
  }
}

NodeComposite *FlowScene::nodeComposite(const QUuid &uuid) {
  if (auto found = nodeComposites_.find(uuid); found != nodeComposites_.end()) {
    return found->second;
  }
  return nullptr;
}

} // namespace QtNodes
