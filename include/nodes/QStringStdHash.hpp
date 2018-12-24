#pragma once

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <functional>

namespace std {
template <> struct hash<QString> {
  inline std::size_t operator()(QString const &s) const { return qHash(s); }
};
} // namespace std
