#include "tadakichiwidget.h"

namespace Tadakichi {
  Widget::Widget(QWidget *parent) : QWidget(parent) {
    auto *lay = new QVBoxLayout(this);
    pic = new QLabel(this);
    pic->setAlignment(Qt::AlignCenter);
    pic->setMinimumSize(120, 120);
    label = new QLabel(tr("Tadakichi is napping..."), this);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    lay->addWidget(pic);
    lay->addWidget(label);
    setLayout(lay);
    bounceTimer = new QTimer(this);
    bounceTimer->setInterval(140);
    connect(bounceTimer, &QTimer::timeout, this, [this]() {
      if (bounceBase.isNull()) {
        return;
      }
      bounceTick++;
      const int s = (bounceTick % 2 == 0) ? 220 : 234;
      pic->setPixmap(bounceBase.scaled(QSize(s, s), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });
    onStopped();
  }

  void Widget::onStarted(const Track &) {
    setMood(QStringLiteral(":/app/resources/tadakichi/playing.jpg"), tr("☆ Tadakichi is vibing ☆"));
    QPixmap pm(QStringLiteral(":/app/resources/tadakichi/playing.jpg"));
    if (!pm.isNull()) {
      bounceBase = pm;
      bounceTick = 0;
      bounceTimer->setInterval(int(140.0 / tempo_));
      bounceTimer->start();
    }
  }

  void Widget::setTempo(double speed) {
    tempo_ = qBound(0.5, speed, 2.0);
    if (bounceTimer->isActive()) {
      bounceTimer->setInterval(int(140.0 / tempo_));
    }
  }

  void Widget::onPaused(const Track &) {
    setMood(QStringLiteral(":/app/resources/tadakichi/paused.gif"), tr("☆ Tadakichi waits... ☆"));
  }

  void Widget::onStopped() {
    setMood(QStringLiteral(":/app/resources/tadakichi/stopped.gif"), tr("☆ Tadakichi is napping... ☆"));
  }

  void Widget::setMood(const QString &res, const QString &text) {
    label->setText(text);
    bounceTimer->stop();
    bounceBase = QPixmap();
    if (movie) {
      movie->stop();
      movie->deleteLater();
      movie = nullptr;
    }
    if (res.endsWith(QStringLiteral(".gif"))) {
      movie = new QMovie(res, QByteArray(), this);
      movie->setScaledSize(QSize(220, 140));
      if (!movie->isValid()) {
        QPixmap fallback(res);
        movie->deleteLater();
        movie = nullptr;
        if (!fallback.isNull()) {
          pic->setPixmap(fallback.scaled(QSize(220, 220), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        return;
      }
      pic->setMovie(movie);
      movie->start();
      return;
    }
    QPixmap pm(res);
    if (!pm.isNull()) {
      pic->setPixmap(pm.scaled(QSize(220, 220), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
  }
}
