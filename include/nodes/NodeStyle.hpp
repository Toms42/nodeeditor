#pragma once

#include "Export.hpp"
#include "Style.hpp"
#include <QtGui/QColor>

namespace QtNodes {

/**\warning you can not inheritance this class, because in konstructor uses
 * virtual functions
 */
class NODE_EDITOR_PUBLIC NodeStyle final : public Style {
public:
  NodeStyle();

  NodeStyle(const QString &jsonText);

public:
  static void setNodeStyle(const QString &jsonText);

private:
  void loadJsonText(const QString &jsonText) override;

  void loadJsonFile(const QString &fileName) override;

  void loadJsonFromByteArray(QByteArray const &byteArray) override;

public:
  QColor NormalBoundaryColor;
  QColor SelectedBoundaryColor;
  QColor GradientColor0;
  QColor GradientColor1;
  QColor GradientColor2;
  QColor GradientColor3;
  QColor ShadowColor;
  QColor FontColor;
  QColor FontColorFaded;

  QColor ConnectionPointColor;
  QColor FilledConnectionPointColor;

  QColor WarningColor;
  QColor ErrorColor;

  float PenWidth;
  float HoveredPenWidth;

  float ConnectionPointDiameter;

  float Opacity;
};
} // namespace QtNodes
