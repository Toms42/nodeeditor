#include "ImageLoaderModel.hpp"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>

ImageLoaderModel::ImageLoaderModel()
    : _label(new QLabel("Double click to load image")) {
  _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  QFont f = _label->font();
  f.setBold(true);
  f.setItalic(true);

  _label->setFont(f);

  _label->setFixedSize(200, 200);

  _label->installEventFilter(this);

  QtNodes::Port port;
  port.captionVisible = false;
  port.dataType       = PixmapData().type();
  port.handle         = [this](std::shared_ptr<NodeData>) {
    return std::make_shared<PixmapData>(_pixmap);
  };

  addPort(PortType::Out, 4, port);
}

ImageLoaderModel::~ImageLoaderModel() {
  delete _label;
}

bool ImageLoaderModel::eventFilter(QObject *object, QEvent *event) {
  if (object == _label) {
    int w = _label->width();
    int h = _label->height();

    if (event->type() == QEvent::MouseButtonPress) {
      QString fileName =
          QFileDialog::getOpenFileName(nullptr,
                                       tr("Open Image"),
                                       QDir::homePath(),
                                       tr("Image Files (*.png *.jpg *.bmp)"));

      _pixmap = QPixmap(fileName);

      _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));

      emit dataUpdated(4);

      return true;
    }
    if (event->type() == QEvent::Resize) {
      if (!_pixmap.isNull()) {
        _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
      }
    }
  }

  return false;
}
