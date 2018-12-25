#pragma once

#include "Export.hpp"
#include "Style.hpp"
#include <QtGui/QColor>

namespace QtNodes {

/**\warning you can not inheritance this class, because in konstructor uses
 * virtual functions
 */
class NODE_EDITOR_PUBLIC FlowViewStyle final : public Style {
public:
  FlowViewStyle();

  FlowViewStyle(const QString &jsonText);

public:
  static void setStyle(const QString &jsonText);

private:
  void loadJsonText(const QString &jsonText) override;

  void loadJsonFile(const QString &fileName) override;

  void loadJsonFromByteArray(QByteArray const &byteArray) override;

public:
  QColor BackgroundColor;
  QColor FineGridColor;
  QColor CoarseGridColor;
};
} // namespace QtNodes
