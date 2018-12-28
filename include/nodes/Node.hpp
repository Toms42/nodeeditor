#pragma once

#include "Export.hpp"
#include "PortType.hpp"
#include "Serializable.hpp"
#include "memory.hpp"
#include <QPointF>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QUuid>

namespace QtNodes {

class Connection;
class ConnectionState;
class NodeGraphicsObject;
class NodeDataModel;
class NodeData;

class NODE_EDITOR_PUBLIC Node
    : public QObject
    , public Serializable {
  Q_OBJECT

public:
  /// NodeDataModel should be an rvalue and is moved into the Node
  Node(std::unique_ptr<NodeDataModel> &&dataModel, QUuid const &id);

  ~Node() override;

public:
  QJsonObject save() const override;

public:
  QUuid id() const;

  // TODO node haven't to now about this
  // QPointF position() const;

  // TODO node haven't to now about this
  // void setPosition(QPointF const &newPos);

public:
  NodeDataModel *nodeDataModel() const;

  std::vector<Connection *> const &connections(PortType  pType,
                                               PortIndex pIdx) const;

  std::vector<Connection *> &connections(PortType pType, PortIndex pIdx);

  /**\brief add new port. Uses by model
   */
  bool addPort(PortType pType, PortIndex pIndex);

  /**\brief remove port. Uses by model
   */
  bool removePort(PortType pType, PortIndex pIndex);

public slots: // data propagation

  /// Propagates incoming data to the underlying model.
  void propagateData(std::shared_ptr<NodeData> nodeData,
                     PortIndex                 inPortIndex) const;

  /// Fetches data from model's OUT #index port
  /// and propagates it to the connection
  void onDataUpdated(PortIndex index);

signals:

  // TODO node haven't to know about this
  // void positionChanged(QPointF const &newPos);

  void portAdded(PortType pType, PortIndex pIndex);

  void portRemoved(PortType pType, PortIndex pIndex);

private:
  // addressing

  QUuid _uid;

  // data
  std::unique_ptr<NodeDataModel>                 _nodeDataModel;
  std::map<PortIndex, std::vector<Connection *>> _inConnections;
  std::map<PortIndex, std::vector<Connection *>> _outConnections;
  // TODO node haven't to know about this
  // QPointF                                        _pos;
};
} // namespace QtNodes
