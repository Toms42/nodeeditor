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
class NodeImp;
class NodeData;

class NODE_EDITOR_PUBLIC Node : public QObject, public Serializable {
  Q_OBJECT

public:
  /// NodeDataModel should be an rvalue and is moved into the Node
  Node(std::unique_ptr<NodeImp> &&dataModel, QUuid const &id);

  virtual ~Node();

public:
  QJsonObject save() const override;

  void restore(QJsonObject const &json) override;

public:
  QUuid id() const;

  QPointF position() const;

  void setPosition(QPointF const &newPos);

public:
  NodeImp *nodeImp() const;

  std::vector<Connection *> const &connections(PortType pType,
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
                     PortIndex inPortIndex) const;

  /// Fetches data from model's OUT #index port
  /// and propagates it to the connection
  void onDataUpdated(PortIndex index);

signals:

  void positionChanged(QPointF const &newPos);

  void portAdded(PortType pType, PortIndex pIndex);

  void portRemoved(PortType pType, PortIndex pIndex);

private:
  // addressing

  QUuid _uid;

  // data
  std::unique_ptr<NodeImp> nodeImp_;
  std::map<PortIndex, std::vector<Connection *>> _inConnections;
  std::map<PortIndex, std::vector<Connection *>> _outConnections;
  QPointF _pos;
};
} // namespace QtNodes
