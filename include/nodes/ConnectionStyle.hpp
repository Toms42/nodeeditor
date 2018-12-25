#pragma once

#include "Export.hpp"
#include "Style.hpp"
#include <QtGui/QColor>

namespace QtNodes {

/**\warning you can not inheritance this class, because in konstructor uses
 * virtual functions
 */
class NODE_EDITOR_PUBLIC ConnectionStyle final : public Style {
public:
  ConnectionStyle();

  ConnectionStyle(const QString &jsonText);

public:
  static void setConnectionStyle(const QString &jsonText);

private:
  void loadJsonText(const QString &jsonText) override;

  void loadJsonFile(const QString &fileName) override;

  void loadJsonFromByteArray(QByteArray const &byteArray) override;

public:
  QColor constructionColor() const;

  QColor normalColor() const;

  QColor normalColor(const QString &typeId) const;

  QColor selectedColor() const;

  QColor selectedHaloColor() const;

  QColor hoveredColor() const;

  float lineWidth() const;

  float constructionLineWidth() const;

  float pointDiameter() const;

  bool useDataDefinedColors() const;

private:
  QColor ConstructionColor;
  QColor NormalColor;
  QColor SelectedColor;
  QColor SelectedHaloColor;
  QColor HoveredColor;

  float LineWidth;
  float ConstructionLineWidth;
  float PointDiameter;

  bool UseDataDefinedColors;
};
} // namespace QtNodes
