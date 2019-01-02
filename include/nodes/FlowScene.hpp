#pragma once

#include "ConnectionID.hpp"
#include "Export.hpp"
#include "QUuidStdHash.hpp"
#include "memory.hpp"
#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>
#include <functional>
#include <tuple>
#include <unordered_map>

class QMenu;

namespace QtNodes {

class FlowSceneModel;
class ConnectionGraphicsObject;
class NodeComposite;
class NodeIndex;
class NodeGraphicsObject;

/// The FlowScene is responsible for rendering a FlowSceneModel
/// If you're looking for a basic dataflow model, see DataFlowScene
class NODE_EDITOR_PUBLIC FlowScene : public QGraphicsScene {
  Q_OBJECT

public:
  FlowScene(FlowSceneModel *model, QObject *parent = Q_NULLPTR);

  ~FlowScene() override;

public:
  FlowSceneModel *model() const { return _model; }

  NodeComposite *nodeComposite(const NodeIndex &index);

  NodeComposite *nodeComposite(const QUuid &uuid);

  std::vector<NodeIndex> selectedNodes() const;

  /**\return temp connection. Only one exempljar per time
   */
  ConnectionGraphicsObject *temporaryConn() const;

  /**\brief set temp connection. Also add it to scene
   * \warning not set ownershiep
   */
  void setTemporaryConn(ConnectionGraphicsObject *conn);

  /**\brief create context menu of scene
   * \warning this menu dynamicaly allocated, so its have ot be deleted after
   * use
   */
  QMenu *createContextMenu();

protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *conn) override;

  /**\brief recursively remove items from model
   */
  void recursivelyRemove(NodeComposite *obj);

signals:
  void addNode(QString modelName);

  /**\brief send signal to model for remove item
   */
  void removeNode(QUuid index);

  void removeConnection(const NodeIndex &leftNode,
                        PortIndex        leftPortID,
                        const NodeIndex &rightNode,
                        PortIndex        rightPortID);

public slots:
  void deleteSelectedNodes();

protected slots:

  void nodeRemoved(const QUuid &id);

  // this slot we overload in DataFlowScene, and set pos and size for its
  virtual void nodeAdded(const QUuid &newID);

  // void nodePortUpdated(NodeIndex const &id);

  void nodeValidationUpdated(NodeIndex const &id);

  void connectionRemoved(NodeIndex const &leftNode,
                         PortIndex        leftPortID,
                         NodeIndex const &rightNode,
                         PortIndex        rightPortID);

  void connectionAdded(NodeIndex const &leftNode,
                       PortIndex        leftPortID,
                       NodeIndex const &rightNode,
                       PortIndex        rightPortID);

  void updateNode(const NodeIndex &nodeIndex);

  void updateConnection(const NodeIndex &leftNodeIndex,
                        PortIndex        leftPortIndex,
                        const NodeIndex &rightNodeIndex,
                        PortIndex        rightPortIndex);

  NodeComposite *createComposite(const NodeIndex &index);

private:
  FlowSceneModel *_model;

  std::unordered_map<QUuid, class NodeComposite *> nodeComposites_;

  std::unordered_map<ConnectionID, ConnectionGraphicsObject *>
      _connGraphicsObjects;

  // This is for when you're creating a connection
  ConnectionGraphicsObject *_temporaryConn = nullptr;

  QPointF lastPos_;
};

NodeComposite *locateNodeAt(QPointF           scenePoint,
                            FlowScene &       scene,
                            QTransform const &viewTransform);
} // namespace QtNodes
