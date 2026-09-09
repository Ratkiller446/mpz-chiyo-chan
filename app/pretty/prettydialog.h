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
  struct Tidy {
    Track track;
    QString field;
    QString oldValue;
    QString newValue;
  };

  class Dialog : public QDialog {
    Q_OBJECT
  public:
    using ApplyFn = std::function<void(const QVector<Track> &)>;
    explicit Dialog(const QString &playlistName, const QVector<Track> &tracks,
                    ApplyFn onApply, QWidget *parent = nullptr);

  signals:
    void tagsSaved(const QList<quint64> &uids);

  private slots:
    void apply();

  private:
    static QString tidied(const QString &s);

    QListWidget *dupeList = nullptr;
    QListWidget *tidyList = nullptr;
    QPushButton *applyButton = nullptr;
    QVector<Track> dupes_;
    QVector<Tidy> tidies_;
    ApplyFn onApply_;
  };
}

#endif
