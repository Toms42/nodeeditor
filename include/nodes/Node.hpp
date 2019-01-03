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
class NodePainterDelegate;

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

  void restore(const QJsonObject &obj) override;

public:
  QUuid id() const;

  // TODO node haven't to now about this
  // QPointF position() const;

  // TODO node haven't to now about this
  // void setPosition(QPointF const &newPos);

public:
  NodeDataModel *nodeDataModel() const;

  QWidget *nodeWidget() const;

  ConnectionPolicy nodePortConnectionPolicy(PortType  type,
                                            PortIndex index) const;

  unsigned nodePortCount(PortType type);

  std::list<PortIndex> nodePortIndexes(PortType type);

  NodeDataType nodePortDataType(PortType type, PortIndex portIndex) const;

  QString nodeCaption() const;

  QString nodePortCaption(PortType type, PortIndex index) const;

  bool nodePortCaptionVisibility(PortType type, PortIndex index) const;

  NodeValidationState nodeValidationState() const;

  QString nodeValidationMessage() const;

  NodePainterDelegate *nodePainterDelegate() const;

  std::list<std::shared_ptr<Connection>> const &
  connections(PortType pType, PortIndex pIdx) const;

  std::list<std::shared_ptr<Connection>> &connections(PortType  pType,
                                                      PortIndex pIdx);

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

  void portAdded(PortType pType, PortIndex pIndex);

  void portRemoved(PortType pType, PortIndex pIndex);

  void widgetChanged();

private:
  // addressing

  QUuid _uid;

  // data
  std::unique_ptr<NodeDataModel>                              _nodeDataModel;
  std::map<PortIndex, std::list<std::shared_ptr<Connection>>> _inConnections;
  std::map<PortIndex, std::list<std::shared_ptr<Connection>>> _outConnections;
  // TODO node haven't to know about this
  // QPointF                                        _pos;
};
} // namespace QtNodes
