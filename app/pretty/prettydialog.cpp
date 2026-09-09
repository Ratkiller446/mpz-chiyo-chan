#include "prettydialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidgetItem>
#include <QMap>

namespace Pretty {
  Dialog::Dialog(const QString &playlistName, const QVector<Track> &tracks,
                 ApplyFn onApply, QWidget *parent)
      : QDialog(parent), onApply_(std::move(onApply)) {
    setWindowTitle(tr("Make Everything Pretty"));
    auto *lay = new QVBoxLayout(this);

    QMap<QString, QVector<Track>> groups;
    for (const auto &t : tracks) {
      const QString key = t.artist().trimmed().toLower() + QStringLiteral("\x1f") +
                          t.album().trimmed().toLower() + QStringLiteral("\x1f") +
                          t.title().trimmed().toLower();
      groups[key] << t;
    }
    for (auto it = groups.cbegin(); it != groups.cend(); ++it) {
      if (it.value().size() > 1) {
        dupes_ << it.value().mid(1);
      }
    }

    auto *info = new QLabel(
        dupes_.isEmpty()
            ? tr("Playlist \"%1\" is already spotless. Chiyo approves!").arg(playlistName)
            : tr("%1 duplicate tracks in \"%2\" (keeping the first of each). Uncheck any to keep.")
                  .arg(dupes_.size())
                  .arg(playlistName),
        this);
    info->setWordWrap(true);
    lay->addWidget(info);

    list = new QListWidget(this);
    for (const auto &t : dupes_) {
      auto *item = new QListWidgetItem(
          QStringLiteral("%1 - %2").arg(t.artist(), t.title()), list);
      item->setCheckState(Qt::Checked);
    }
    lay->addWidget(list);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel, this);
    applyButton = buttons->button(QDialogButtonBox::Apply);
    applyButton->setEnabled(!dupes_.isEmpty());
    connect(buttons, &QDialogButtonBox::accepted, this, &Dialog::apply);
    connect(buttons, &QDialogButtonBox::rejected, this, &Dialog::reject);
    lay->addWidget(buttons);
    setLayout(lay);
    resize(480, 360);
  }

  void Dialog::apply() {
    QVector<Track> doomed;
    for (int i = 0; i < list->count(); i++) {
      if (list->item(i)->checkState() == Qt::Checked) {
        doomed << dupes_[i];
      }
    }
    if (!doomed.isEmpty() && onApply_) {
      onApply_(doomed);
    }
    accept();
  }
}
