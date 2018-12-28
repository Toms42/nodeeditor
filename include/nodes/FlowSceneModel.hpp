#pragma once

#include "Export.hpp"
#include "NodeStyle.hpp"
#include "PortType.hpp"
#include "types.hpp"
#include <QList>
#include <QObject>
#include <QString>
#include <QUuid>
#include <cstddef>

namespace QtNodes {

class NodeIndex;
struct NodeDataType;
class NodePainterDelegate;

class NODE_EDITOR_PUBLIC FlowSceneModel : public QObject {
  Q_OBJECT

public:
  FlowSceneModel() = default;

  // Scene specific functions
  virtual QStringList modelRegistry() const = 0;

  /// Get the catergory for a node type
  /// name will be from `modelRegistry()`
  virtual QString nodeTypeCategory(QString const & /*name*/) const {
    return {};
  }

  /// Get if two types are convertable
  virtual bool getTypeConvertable(TypeConverterId const & /*id*/) const {
    return false;
  }

  // Retrieval functions
  //////////////////////

  /// Get the list of node IDs
  virtual QList<QUuid> nodeUUids() const = 0;

  /// Create a NodeIndex for a node
  virtual NodeIndex nodeIndex(const QUuid &ID) const = 0;

  /// Get the type ID for the node
  virtual QString nodeTypeIdentifier(NodeIndex const &index) const = 0;

  /// Get the caption for the node
  virtual QString nodeCaption(NodeIndex const &index) const = 0;

  /// Get the location of a node
  // virtual QPointF nodeLocation(NodeIndex const &index) const = 0;

  /// Get the embedded widget
  virtual QWidget *nodeWidget(NodeIndex const &index) const = 0;

  /// Get if it's resizable
  // virtual bool nodeResizable(NodeIndex const &index) const = 0;

  /// Get the validation state
  virtual NodeValidationState
  nodeValidationState(NodeIndex const &index) const = 0;

  /// Get the validation error/warning
  virtual QString nodeValidationMessage(NodeIndex const &index) const = 0;

  /// Get the painter delegate
  virtual NodePainterDelegate *
  nodePainterDelegate(NodeIndex const &index) const = 0;

  /// Get the style - {} for default
  virtual NodeStyle nodeStyle(NodeIndex const & /*index*/) const { return {}; }

  /// Get the count of DataPorts
  virtual unsigned int nodePortCount(NodeIndex const &index,
                                     PortType         portType) const = 0;

  virtual std::list<PortIndex> nodePortIndexes(NodeIndex const &index,
                                               PortType portType) const = 0;

  /// Get the port caption
  virtual QString nodePortCaption(NodeIndex const &index,
                                  PortIndex        portID,
                                  PortType         portType) const = 0;

  /// Get the port data type
  virtual NodeDataType nodePortDataType(NodeIndex const &index,
                                        PortIndex        portID,
                                        PortType         portType) const = 0;

  /// Port Policy
  virtual ConnectionPolicy nodePortConnectionPolicy(
      NodeIndex const &index, PortIndex portID, PortType portType) const = 0;

  /// Get a connection at a port
  /// Returns the remote node and the remote port index for that node
  virtual std::vector<std::pair<NodeIndex, PortIndex>> nodePortConnections(
      NodeIndex const &index, PortIndex portID, PortType portTypes) const = 0;

  // Mutation functions
  /////////////////////

public slots:
  /// Remove a connection
  virtual bool removeConnection(NodeIndex const & /*leftNode*/,
                                PortIndex /*leftPortID*/,
                                NodeIndex const & /*rightNode*/,
                                PortIndex /*rightPortID*/) = 0;

public:
  virtual bool removePort(const NodeIndex &nodeIndex,
                          PortType         portType,
                          PortIndex        portIndex) = 0;

  /// Add a connection
  virtual bool addConnection(NodeIndex const & /*leftNode*/,
                             PortIndex /*leftPortID*/,
                             NodeIndex const & /*rightNode*/,
                             PortIndex /*rightPortID*/) = 0;

  /// Remove a node
  virtual bool removeNode(NodeIndex const & /*index*/) = 0;

public:
  /// Add a  -- return {} if it fails
  virtual QUuid addNode(QString const & /*typeID*/) { return QUuid{}; }

  virtual bool
  addPort(const NodeIndex &nIndex, PortType pType, PortIndex pIndex) = 0;

  /// Move a node to a new location
  // virtual bool moveNode(NodeIndex const & /*index*/, QPointF /*newLocation*/)
  // {
  //  return false;
  //}

public slots:
  /// Helper functions
  ////////////////////

  // try to remove all connections and then the node
  bool removeNodeWithConnections(NodeIndex const &index);

public:
  /// Notifications
  /////////////////

  // TODO all this have to do scene - not model
  // virtual void connectionHovered(NodeIndex const & /*lhs*/,
  //                               PortIndex /*lPortIndex*/,
  //                               NodeIndex const & /*rhs*/,
  //                               PortIndex /*rPortIndex*/,
  //                               QPoint const & /*pos*/,
  //                               bool /*entered*/) {}

  // virtual void nodeHovered(NodeIndex const & /*index*/,
  //                         QPoint const & /*pos*/,
  //                         bool /*entered*/) {}

  // virtual void nodeDoubleClicked(NodeIndex const & /*index*/,
  //                               QPoint const & /*pos*/) {}

  // virtual void nodeContextMenu(NodeIndex const & /*index*/,
  //                             QPoint const & /*pos*/) {}

signals:

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
  // TODO model haven't to know about this
  // void nodeMoved(NodeIndex const &index);
  void connectionsRemoved(PortType type, PortIndex index);
  void updateNode(const NodeIndex &nodeIndex);
  void updateConnection(const NodeIndex &leftNodeIndex,
                        PortIndex        leftPortIndex,
                        const NodeIndex &rightNodeIndex,
                        PortIndex        rightPortIndex);

protected:
  NodeIndex createIndex(const QUuid &id, void *internalPointer) const;
  /**\brief create not valid index with @id
   */
  NodeIndex createNotValidIndex(const QUuid &id) const;
};

} // namespace QtNodes
