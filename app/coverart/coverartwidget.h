#ifndef COVERART_WIDGET_H
#define COVERART_WIDGET_H

#include "track.h"

#include <QLabel>
#include <QPixmap>
#include <QString>

namespace Config {
  class Global;
}

namespace CoverArt {
  class Widget : public QLabel {
    Q_OBJECT
  public:
    explicit Widget(Config::Global &conf, QWidget *parent = nullptr);

  public slots:
    void setTrack(const Track &track);
    void clear();
    void setPlaceholder(const QString &path);

  signals:
    void trackInfoRequested(const Track &track);

  protected:
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

  private slots:
    void showContextMenu(const QPoint &pos);
    void onSearchStarted(const QString &artist, const QString &album);
    void onCoverDownloaded(const QString &artist, const QString &album, const QString &path);
    void onSearchFinished(const QString &artist, const QString &album);

  private:
    void render();
    void render_cover();
    bool render_placeholder();
    bool isCurrent(const QString &artist, const QString &album) const;

    QPixmap source;
    QPixmap placeholder;
    Track _track;
    QString _cover_path;
    Config::Global &_conf;
    double _zoom = 1.0; // ponytail: wheel zoom, double-click resets; clamp 0.2-4x
  };
}

#endif // COVERART_WIDGET_H
