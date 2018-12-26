// NodeImp.hpp

#pragma once

#include "Export.hpp"
#include "NodeStyle.hpp"
#include "Port.hpp"
#include "PortType.hpp"
#include "Serializable.hpp"
#include "types.hpp"
#include <QObject>
#include <QString>

namespace QtNodes {

class NodeStyle;
class Node;
class NodePainterDelegate;

class NODE_EDITOR_PUBLIC NodeDataModel
    : public QObject
    , public Serializable {
  Q_OBJECT

  friend Node;

public:
  NodeDataModel();
  ~NodeDataModel() override;

  virtual QString name() const = 0;

  /**\brief add new port in node
   * \return true if added, false - otherwise
   */
  bool addPort(PortType type, PortIndex index, Port port);

  /**\brief remove port from node
   * \return true if removed, false - otherwise
   */
  bool removePort(PortType type, PortIndex index);

  const NodeStyle &nodeStyle() const;
  void             setNodeStyle(const NodeStyle &style);

  /**\brief this method uses Port::handle for handle input data
   */
  void setInData(std::shared_ptr<NodeData> nodeData, PortIndex index);

  /**\return rezult of Port::handle(nullptr)
   */
  std::shared_ptr<NodeData> outData(PortIndex index);

  /**\return QWidget which will be desplayed
   * \warning return widget must haven't parent!
   */
  virtual QWidget *embeddedWidget() = 0;

  /**\return by default true
   */
  virtual bool resizable() const;

  /**\return current validation state of node. By default Valid
   */
  virtual NodeValidationState validationState() const;

  /**\return validation message. By default ""
   */
  virtual QString validationMessage() const;

  QJsonObject save() const override;

  NodeDataType dataType(PortType portType, PortIndex portIndex) const;

  /**\return capcity of ports with @type
   */
  unsigned int nPorts(PortType type) const;

  /**\return all ports indexes
   */
  std::list<PortIndex> ports(PortType type) const;

  /**\return caption of port
   */
  QString portCaption(PortType type, PortIndex index) const;

  bool portCaptionVisibility(PortType type, PortIndex index) const;

  /**\return current out connection policy. By default Many
   */
  virtual ConnectionPolicy portOutConnectionPolicy(PortIndex) const;

  virtual NodePainterDelegate *painterDelegate() const;

signals:
  void dataUpdated(PortIndex index);

  void dataInvalidated(PortIndex index);

  void computingStarted();

  void computingFinished();

  void portAdded(PortType type, PortIndex index);

  void portRemoved(PortType type, PortIndex index);

private:
  NodeStyle                 nodeStyle_;
  std::map<PortIndex, Port> inPorts_;
  std::map<PortIndex, Port> outPorts_;
};

}; // namespace QtNodes
