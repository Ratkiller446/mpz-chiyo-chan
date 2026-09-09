#ifndef TADAKICHI_WIDGET_H
#define TADAKICHI_WIDGET_H

#include "track.h"

#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace Tadakichi {
  class Widget : public QWidget {
    Q_OBJECT
  public:
    explicit Widget(QWidget *parent = nullptr);

  public slots:
    void onStarted(const Track &track);
    void onPaused(const Track &track);
    void onStopped();

  private:
    void setMood(const QString &res, const QString &text);

    QLabel *pic = nullptr;
    QLabel *label = nullptr;
    QMovie *movie = nullptr;
    QTimer *bounceTimer = nullptr;
    QPixmap bounceBase;
    int bounceTick = 0;
  };
}

#endif
