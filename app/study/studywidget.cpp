#include "studywidget.h"

#include "playback/playbackcontroller.h"

#include <QDockWidget>

namespace Study {
  Widget::Widget(QWidget *parent) : QWidget(parent) {
    auto *lay = new QVBoxLayout(this);
    sessionLabel = new QLabel(tr("No session"), this);
    sessionLabel->setAlignment(Qt::AlignCenter);
    toggle = new QPushButton(tr("Start studying"), this);
    hideBox = new QCheckBox(tr("Hide distractions"), this);
    hideBox->setChecked(true);
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(toggle, &QPushButton::clicked, this, &Widget::toggleSession);
    connect(timer, &QTimer::timeout, this, &Widget::tick);
    lay->addWidget(sessionLabel);
    lay->addWidget(toggle);
    lay->addWidget(hideBox);
    setLayout(lay);
  }

  void Widget::setup(const QList<QDockWidget *> &docks, Playback::Controller *player) {
    docks_ = docks;
    player_ = player;
  }

  void Widget::toggleSession() {
    if (!active) {
      active = true;
      elapsed = 0;
      sessionLabel->setText(tr("Study session: 00:00"));
      toggle->setText(tr("Stop studying"));
      if (player_) {
        savedVolume = player_->volume();
        if (savedVolume > 60) {
          player_->setVolume(60);
        }
      }
      hidden_.clear();
      if (hideBox->isChecked()) {
        for (auto *d : docks_) {
          if (d && d->isVisible()) {
            hidden_ << d;
            d->hide();
          }
        }
      }
      timer->start();
      return;
    }
    active = false;
    timer->stop();
    if (player_ && savedVolume >= 0) {
      player_->setVolume(savedVolume);
      savedVolume = -1;
    }
    for (auto *d : hidden_) {
      if (d) {
        d->show();
      }
    }
    hidden_.clear();
    sessionLabel->setText(tr("No session"));
    toggle->setText(tr("Start studying"));
  }

  void Widget::tick() {
    sessionLabel->setText(tr("Study session: %1").arg(fmt(++elapsed)));
  }

  QString Widget::fmt(int secs) {
    return QStringLiteral("%1:%2")
        .arg(secs / 60, 2, 10, QLatin1Char('0'))
        .arg(secs % 60, 2, 10, QLatin1Char('0'));
  }
}
