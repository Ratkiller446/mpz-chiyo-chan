#ifndef THEMES_H
#define THEMES_H

#include <QString>
#include <QStringList>

namespace Themes {
  QStringList names();
  void apply(const QString &name);
}

#endif
