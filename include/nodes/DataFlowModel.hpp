// DataFlowModel.hpp

#pragma once

#include "ConnectionID.hpp"
#include "Export.hpp"
#include "FlowSceneModel.hpp"
#include "Node.hpp"
#include "NodeIndex.hpp"
#include "QUuidStdHash.hpp"
#include "TypeConverter.hpp"
#include "types.hpp"
#include <functional>
#include <memory>
#include <unordered_map>

namespace QtNodes {
class DataModelRegistry;
class NodeDataModel;
class Connection;
class Node;

/// Model for simple data flow
class NODE_EDITOR_PUBLIC DataFlowModel : public FlowSceneModel {
  Q_OBJECT

public:
  DataFlowModel(std::shared_ptr<DataModelRegistry> reg,
                QObject *                          parent = nullptr);
  ~DataFlowModel() override;

  // FlowSceneModel read interface
  QStringList modelRegistry() const override;

  QString nodeTypeCategory(QString const &name) const override;

  bool getTypeConvertable(TypeConverterId const &id) const override;

  QList<QUuid> nodeUUids() const override;

  /**\return if model contains node with @ID - return valid index, if not -
   * return not valid NodeIndex with @ID
   */
  NodeIndex nodeIndex(const QUuid &ID) const override;

  QString nodeTypeIdentifier(NodeIndex const &index) const override;

  QString nodeCaption(NodeIndex const &index) const override;

  QWidget *nodeWidget(NodeIndex const &index) const override;

  NodeValidationState
  nodeValidationState(NodeIndex const &index) const override;

  QString nodeValidationMessage(NodeIndex const &index) const override;

  NodePainterDelegate *
  nodePainterDelegate(NodeIndex const &index) const override;

  unsigned int nodePortCount(NodeIndex const &index,
                             PortType         portType) const override;

  std::list<PortIndex> nodePortIndexes(const NodeIndex &index,
                                       PortType portType) const override;

  QString nodePortCaption(NodeIndex const &index,
                          PortIndex        pIndex,
                          PortType         portType) const override;

  NodeDataType nodePortDataType(NodeIndex const &index,
                                PortIndex        pIndex,
                                PortType         portType) const override;

  ConnectionPolicy nodePortConnectionPolicy(NodeIndex const &index,
                                            PortIndex        pIndex,
                                            PortType portType) const override;

  std::vector<std::pair<NodeIndex, PortIndex>> nodePortConnections(
      NodeIndex const &index, PortIndex id, PortType portType) const override;

  bool removePort(const NodeIndex &nodeIndex,
                  PortType         type,
                  PortIndex        index) override;

  // FlowSceneModel write interface
  bool removeConnection(NodeIndex const &leftNode,
                        PortIndex        leftPortID,
                        NodeIndex const &rightNode,
                        PortIndex        rightPortID) override;
  bool addConnection(NodeIndex const &leftNode,
                     PortIndex        leftPortID,
                     NodeIndex const &rightNode,
                     PortIndex        rightPortID) override;

  bool removeNode(QUuid uuid) override;

  QByteArray saveToMemory() const;

  void loadFromMemory(const QByteArray &data);

  void restoreNode(const QJsonObject &data);

  void restoreConnection(const QJsonObject &data);

public slots:
  QUuid addNode(QString const &typeID) override;

public:
  bool
  addPort(const NodeIndex &nIndex, PortType pType, PortIndex pIndex) override;

  // convenience functions
  QUuid        addNode(QString const &typeID, QUuid const &uuid);
  QUuid        addNode(std::unique_ptr<NodeDataModel> &&model,
                       QUuid const &                    uuid = QUuid::createUuid());
  ConnectionID addConnection(Node *        left,
                             PortIndex     leftIdx,
                             Node *        right,
                             PortIndex     rightIdx,
                             TypeConverter converter);

  using SharedConnection = std::shared_ptr<Connection>;
  using UniqueNode       = std::unique_ptr<Node>;

  std::unordered_map<ConnectionID, SharedConnection> &connections();

  std::unordered_map<QUuid, UniqueNode> &nodes();

  DataModelRegistry &registry();

  void setRegistry(std::shared_ptr<DataModelRegistry> registry);

private:
  std::unordered_map<QUuid, UniqueNode>              _nodes;
  std::unordered_map<ConnectionID, SharedConnection> _connections;
  std::shared_ptr<DataModelRegistry>                 _registry;
};
}; // namespace QtNodes
