#ifndef RECIPE_WIDGET_H
#define RECIPE_WIDGET_H

#include "track.h"

#include <QLabel>
#include <QListWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

namespace Recipes {
  struct Card {
    QString name;
    QString time;
    QString ingredients;
    QString steps;
  };

  const QVector<Card> &cards();

  class Widget : public QWidget {
    Q_OBJECT
  public:
    explicit Widget(QWidget *parent = nullptr);

  public slots:
    void onStarted(const Track &track);
    void onStopped();

  private slots:
    void showCard(int row);

  private:
    QLabel *nowPlaying = nullptr;
    QListWidget *list = nullptr;
    QTextBrowser *detail = nullptr;
  };
}

#endif
