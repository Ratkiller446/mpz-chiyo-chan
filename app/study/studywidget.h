#ifndef STUDY_WIDGET_H
#define STUDY_WIDGET_H

#include <QCheckBox>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

class QDockWidget;

namespace Playback {
  class Controller;
}

namespace Study {
  class Widget : public QWidget {
    Q_OBJECT
  public:
    explicit Widget(QWidget *parent = nullptr);
    void setup(const QList<QDockWidget *> &docks, Playback::Controller *player);

  private slots:
    void toggleSession();
    void tick();

  private:
    static QString fmt(int secs);

    QLabel *sessionLabel = nullptr;
    QPushButton *toggle = nullptr;
    QCheckBox *hideBox = nullptr;
    QTimer *timer = nullptr;
    QList<QDockWidget *> docks_;
    QList<QDockWidget *> hidden_;
    Playback::Controller *player_ = nullptr;
    int savedVolume = -1;
    int elapsed = 0;
    bool active = false;
  };
}

#endif
