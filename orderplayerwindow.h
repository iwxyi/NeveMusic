#ifndef ORDERPLAYERWINDOW_H
#define ORDERPLAYERWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QNetworkCookie>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextCodec>
#include <stdio.h>
#include <iostream>
#include <QtWebSockets/QWebSocket>
#include <QAuthenticator>
#include <QtConcurrent/QtConcurrent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStyledItemDelegate>
#include <QListView>
#include <QStringListModel>
#include <QScrollBar>
#include "facilemenu.h"

QT_BEGIN_NAMESPACE
namespace Ui { class OrderPlayerWindow; }
QT_END_NAMESPACE

#define JSON_VAL_LONG(json, x) static_cast<qint64>(json.value(#x).toDouble())
#define JVAL_LONG(x) static_cast<qint64>(json.value(#x).toDouble())
#define JVAL_INT(x) json.value(#x).toInt()
#define JVAL_STR(x) json.value(#x).toString()
#define snum(x) QString::number(x)

struct Artist
{
    qint64 id;
    QString name;
    QString faceUrl;

    static Artist fromJson(QJsonObject json)
    {
        Artist artist;
        artist.id = JVAL_LONG(id);
        artist.name = JVAL_STR(name);
        artist.faceUrl = JVAL_STR(img1v1Url);
        return artist;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("id", id);
        json.insert("name", name);
        json.insert("faceUrl", faceUrl);
        return json;
    }
};

struct Album
{
    qint64 id;
    QString name;
    int size;
    int mark;

    static Album fromJson(QJsonObject json)
    {
        Album album;
        album.id = JVAL_LONG(id);
        album.name = JVAL_STR(name);
        album.size = JVAL_INT(size);
        album.mark = JVAL_INT(mark);
        return album;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("id", id);
        json.insert("name", name);
        json.insert("size", size);
        json.insert("mark", mark);
        return json;
    }
};

struct Song
{
    qint64 id;
    QString name;
    int duration;
    int mark;
    QList<Artist> artists;
    Album album;
    QString artistNames;

    static Song fromJson(QJsonObject json)
    {
        Song song;
        song.id = JVAL_LONG(id);
        song.name = JVAL_STR(name);
        QJsonArray array = json.value("artists").toArray();
        QStringList artistNameList;
        foreach (QJsonValue val, array)
        {
            Artist artist = Artist::fromJson(val.toObject());
            song.artists.append(artist);
            artistNameList.append(artist.name);
        }
        song.artistNames = artistNameList.join("/");
        song.album = Album::fromJson(json.value("album").toObject());
        song.duration = JVAL_INT(duration);
        song.mark = JVAL_INT(mark);
        return song;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("id", id);
        json.insert("name", name);
        json.insert("duration", duration);
        json.insert("mark", mark);
        QJsonArray array;
        foreach (Artist artist, artists)
            array.append(artist.toJson());
        json.insert("artists", array);
        json.insert("album", album.toJson());
        return json;
    }

    bool isValid() const
    {
        return id;
    }

    bool operator==(const Song& song)
    {
        return this->id == song.id;
    }

    QString simpleString() const
    {
        return name + " - " + artistNames;
    }
};

typedef QList<Song> SongList;

class OrderPlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    OrderPlayerWindow(QWidget *parent = nullptr);
    ~OrderPlayerWindow();

private slots:
    void on_searchEdit_returnPressed();

    void on_searchButton_clicked();

    void on_searchResultTable_cellActivated(int row, int);

    void on_searchResultTable_customContextMenuRequested(const QPoint &);

    void on_listTabWidget_currentChanged(int index);

private:
    void searchMusic(QString key);
    void setSearchResultTable(QList<Song> songs);
    void addFavorite(SongList songs);
    void removeFavorite(SongList songs);
    void saveSongList(QString key, SongList songs);
    void restoreSongList(QString key, SongList& songs);
    void setSongModelToView(const SongList& songs, QListView* listView);

    void startPlaySong(Song song);
    void appendOrderSongs(SongList songs);
    void appendNextSongs(SongList songs);

    void downloadSong(Song song, bool play = false);

protected:
    void showEvent(QShowEvent*) override;
    void closeEvent(QCloseEvent*) override;

private:
    Ui::OrderPlayerWindow *ui;
    QSettings settings;
    QDir musicsFileDir;
    QList<Song> searchResultSongs;

    SongList orderSongs;
    SongList favoriteSongs;
    SongList historySongs;
};

class NoFocusDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    NoFocusDelegate(){}
    ~NoFocusDelegate(){}

    void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
    {
        QStyleOptionViewItem itemOption(option);
        if (itemOption.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, QColor(100, 149, 237, 88));
            /*int radius = option.rect.height() / 2;
            QPainterPath path;
            path.addRoundedRect(option.rect, radius, radius);
            painter->fillPath(path, QColor(100, 149, 237, 128));*/
        }
        QRect rect = option.rect;
        rect.setLeft(rect.left() + 4);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());
    }
};

#endif // ORDERPLAYERWINDOW_H
