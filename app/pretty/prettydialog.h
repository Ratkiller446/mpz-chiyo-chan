#ifndef PRETTY_DIALOG_H
#define PRETTY_DIALOG_H

#include "track.h"

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

#include <functional>

namespace Pretty {
  class Dialog : public QDialog {
    Q_OBJECT
  public:
    using ApplyFn = std::function<void(const QVector<Track> &)>;
    explicit Dialog(const QString &playlistName, const QVector<Track> &tracks,
                    ApplyFn onApply, QWidget *parent = nullptr);

  private slots:
    void apply();

  private:
    QListWidget *list = nullptr;
    QPushButton *applyButton = nullptr;
    QVector<Track> dupes_;
    ApplyFn onApply_;
  };
}

#endif
