#include "themes.h"

#include <QApplication>
#include <QMap>
#include <QPalette>

namespace Themes {
  namespace {
    struct Colors {
      QString window;
      QString base;
      QString button;
      QString highlight;
      QString text;
    };

    const QMap<QString, Colors> &presets() {
      static const QMap<QString, Colors> all = {
        {QStringLiteral("Sakura"), {QStringLiteral("#FDEEF4"), QStringLiteral("#FFF7FA"), QStringLiteral("#F8D7E3"), QStringLiteral("#F4A7C3"), QStringLiteral("#5A3A44")}},
        {QStringLiteral("Sky-blue"), {QStringLiteral("#EAF4FD"), QStringLiteral("#F5FAFF"), QStringLiteral("#D3E7F8"), QStringLiteral("#7FB8E8"), QStringLiteral("#2E4356")}},
        {QStringLiteral("Soft pastel"), {QStringLiteral("#F1F5E9"), QStringLiteral("#FAFDF4"), QStringLiteral("#DDEBD2"), QStringLiteral("#A8D5BA"), QStringLiteral("#3F4A3C")}},
        {QStringLiteral("Summer vacation"), {QStringLiteral("#FFF3D6"), QStringLiteral("#FFFBEC"), QStringLiteral("#FFE1A8"), QStringLiteral("#F5A623"), QStringLiteral("#5A3A1A")}},
      };
      return all;
    }
  }

  QStringList names() {
    return QStringList{QStringLiteral("Default")} + presets().keys();
  }

  void apply(const QString &name) {
    static const QPalette initial = QApplication::palette();
    if (name.isEmpty() || name == QStringLiteral("Default") || !presets().contains(name)) {
      QApplication::setPalette(initial);
      return;
    }
    const Colors c = presets().value(name);
    QPalette p(initial);
    p.setColor(QPalette::Window, QColor(c.window));
    p.setColor(QPalette::Base, QColor(c.base));
    p.setColor(QPalette::Button, QColor(c.button));
    p.setColor(QPalette::Highlight, QColor(c.highlight));
    p.setColor(QPalette::HighlightedText, QColor(QStringLiteral("#FFFFFF")));
    p.setColor(QPalette::WindowText, QColor(c.text));
    p.setColor(QPalette::Text, QColor(c.text));
    p.setColor(QPalette::ButtonText, QColor(c.text));
    QApplication::setPalette(p);
  }
}
