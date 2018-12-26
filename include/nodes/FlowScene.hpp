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

  std::vector<NodeIndex> selectedNodes() const;

  /**\return temp connection. Only one exempljar per time
   */
  ConnectionGraphicsObject *temporaryConn() const;

  /**\brief set temp connection. Also add it to scene
   * \warning not set ownershiep
   */
  void setTemporaryConn(ConnectionGraphicsObject *conn);

private slots:

  void nodeRemoved(const QUuid &id);

  void nodeAdded(const QUuid &newID);

  void nodePortUpdated(NodeIndex const &id);

  void nodeValidationUpdated(NodeIndex const &id);

  void connectionRemoved(NodeIndex const &leftNode,
                         PortIndex        leftPortID,
                         NodeIndex const &rightNode,
                         PortIndex        rightPortID);

  void connectionAdded(NodeIndex const &leftNode,
                       PortIndex        leftPortID,
                       NodeIndex const &rightNode,
                       PortIndex        rightPortID);

  void nodeMoved(NodeIndex const &index);

  void updateNode(const NodeIndex &nodeIndex);

  void updateConnection(const NodeIndex &leftNodeIndex,
                        PortIndex        leftPortIndex,
                        const NodeIndex &rightNodeIndex,
                        PortIndex        rightPortIndex);

private:
  FlowSceneModel *_model;

  std::unordered_map<QUuid, class NodeComposite *> nodeComposites_;

  std::unordered_map<ConnectionID, ConnectionGraphicsObject *>
      _connGraphicsObjects;

  // This is for when you're creating a connection
  ConnectionGraphicsObject *_temporaryConn = nullptr;
};

NodeComposite *locateNodeAt(QPointF           scenePoint,
                            FlowScene &       scene,
                            QTransform const &viewTransform);
} // namespace QtNodes
