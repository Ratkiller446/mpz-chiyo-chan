#include "prettydialog.h"

#include <fileref.h>
#include <tag.h>
#include <tstring.h>

#include <QDialogButtonBox>
#include <QFile>
#include <QLabel>
#include <QListWidgetItem>
#include <QMap>
#include <QMessageBox>

namespace Pretty {
  namespace {
    TagLib::String to_taglib(const QString &s) {
      const QByteArray utf8 = s.toUtf8();
      return TagLib::String(utf8.constData(), TagLib::String::UTF8);
    }
  }

  QString Dialog::tidied(const QString &s) {
    return s.simplified();
  }

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

    auto *dupeInfo = new QLabel(
        dupes_.isEmpty()
            ? tr("No duplicates in \"%1\". Chiyo approves!").arg(playlistName)
            : tr("%1 duplicate tracks in \"%2\" (keeping the first of each). Uncheck any to keep.")
                  .arg(dupes_.size())
                  .arg(playlistName),
        this);
    dupeInfo->setWordWrap(true);
    lay->addWidget(dupeInfo);

    dupeList = new QListWidget(this);
    for (const auto &t : dupes_) {
      auto *item = new QListWidgetItem(
          QStringLiteral("%1 - %2").arg(t.artist(), t.title()), dupeList);
      item->setCheckState(Qt::Checked);
    }
    lay->addWidget(dupeList);

    for (const auto &t : tracks) {
      const struct {
        QString (Track::*get)() const;
        const char *label;
      } fields[] = {{&Track::artist, "artist"}, {&Track::album, "album"}, {&Track::title, "title"}};
      for (const auto &f : fields) {
        const QString oldV = (t.*(f.get))();
        const QString newV = tidied(oldV);
        if (newV != oldV) {
          tidies_ << Tidy{t, QString::fromLatin1(f.label), oldV, newV};
        }
      }
    }

    auto *tidyInfo = new QLabel(
        tidies_.isEmpty()
            ? tr("All tags are tidy.")
            : tr("%1 messy tags (extra spaces). Uncheck any to skip. Writes to your files!").arg(tidies_.size()),
        this);
    tidyInfo->setWordWrap(true);
    lay->addWidget(tidyInfo);

    tidyList = new QListWidget(this);
    for (const auto &td : tidies_) {
      auto *item = new QListWidgetItem(
          QStringLiteral("%1 - %2: \"%3\" -> \"%4\"")
              .arg(td.track.artist(), td.track.title(), td.oldValue, td.newValue),
          tidyList);
      item->setCheckState(Qt::Checked);
    }
    lay->addWidget(tidyList);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel, this);
    applyButton = buttons->button(QDialogButtonBox::Apply);
    applyButton->setEnabled(!dupes_.isEmpty() || !tidies_.isEmpty());
    connect(buttons, &QDialogButtonBox::accepted, this, &Dialog::apply);
    connect(buttons, &QDialogButtonBox::rejected, this, &Dialog::reject);
    lay->addWidget(buttons);
    setLayout(lay);
    resize(520, 480);
  }

  void Dialog::apply() {
    QVector<Track> doomed;
    for (int i = 0; i < dupeList->count(); i++) {
      if (dupeList->item(i)->checkState() == Qt::Checked) {
        doomed << dupes_[i];
      }
    }
    if (!doomed.isEmpty() && onApply_) {
      onApply_(doomed);
    }

    QList<quint64> fixed;
    int failed = 0;
    for (int i = 0; i < tidyList->count(); i++) {
      if (tidyList->item(i)->checkState() != Qt::Checked) {
        continue;
      }
      const Tidy &td = tidies_[i];
      TagLib::FileRef f(QFile::encodeName(td.track.path()).constData(), false);
      if (f.isNull() || !f.tag()) {
        failed++;
        continue;
      }
      TagLib::Tag *tag = f.tag();
      if (td.field == QStringLiteral("artist")) {
        tag->setArtist(to_taglib(td.newValue));
      } else if (td.field == QStringLiteral("album")) {
        tag->setAlbum(to_taglib(td.newValue));
      } else if (td.field == QStringLiteral("title")) {
        tag->setTitle(to_taglib(td.newValue));
      }
      if (f.save()) {
        fixed << td.track.uid();
      } else {
        failed++;
      }
    }
    if (!fixed.isEmpty()) {
      emit tagsSaved(fixed);
    }
    if (failed > 0) {
      QMessageBox::warning(this, tr("Make Everything Pretty"),
                           tr("%1 tags could not be written (read-only files?).").arg(failed));
    }
    accept();
  }
}
