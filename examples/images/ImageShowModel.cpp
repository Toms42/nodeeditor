#include "ImageShowModel.hpp"
#include "PixmapData.hpp"
#include "nodes/Port.hpp"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry.hpp>

ImageShowModel::ImageShowModel()
    : _label(new QLabel("Image will appear here")) {
  _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  QFont f = _label->font();
  f.setBold(true);
  f.setItalic(true);

  _label->setFont(f);

  _label->setFixedSize(200, 200);

  _label->installEventFilter(this);

  QtNodes::Port portIn;
  portIn.captionVisible = false;
  portIn.dataType       = PixmapData().type();
  portIn.handle         = [this](std::shared_ptr<NodeData> data) {
    _nodeData = data;

    if (_nodeData) {
      auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);

      int w = _label->width();
      int h = _label->height();

      _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
    } else {
      _label->setPixmap(QPixmap());
    }

    emit dataUpdated(5);

    return nullptr;
  };

  addPort(PortType::In, 6, portIn);

  QtNodes::Port portOut;
  portOut.captionVisible = false;
  portOut.dataType       = PixmapData().type();
  portOut.handle = [this](std::shared_ptr<NodeData>) { return _nodeData; };
  addPort(PortType::Out, 5, portOut);
}

ImageShowModel::~ImageShowModel() {
  _label->deleteLater();
}

bool ImageShowModel::eventFilter(QObject *object, QEvent *event) {
  if (object == _label) {
    int w = _label->width();
    int h = _label->height();

    if (event->type() == QEvent::Resize) {
      auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
      if (d) {
        _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
      }
    }
  }

  return false;
}
