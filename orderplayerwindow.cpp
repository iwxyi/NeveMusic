#include "orderplayerwindow.h"
#include "ui_orderplayerwindow.h"

OrderPlayerWindow::OrderPlayerWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::OrderPlayerWindow),
      settings("settings.ini", QSettings::Format::IniFormat),
      musicsFileDir("musics")
{
    ui->setupUi(this);

    QHeaderView* header = ui->searchResultTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setMinimumSectionSize(QFontMetrics(this->font()).horizontalAdvance("哈哈哈哈哈哈"));
    header->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    header->setStyleSheet("QHeaderView { background-color: transparent; }");
    ui->searchResultTable->verticalHeader()->setStyleSheet("QHeaderView { background-color: transparent; }");
    ui->searchResultTable->setItemDelegate(new NoFocusDelegate());

    musicsFileDir.mkpath(musicsFileDir.absolutePath());

    ui->listTabWidget->setCurrentIndex(settings.value("orderplayerwindow/tabIndex").toInt());
    restoreSongList("music/order", orderSongs);
    restoreSongList("music/favorite", favoriteSongs);
    restoreSongList("music/history", historySongs);
    setSongModelToView(orderSongs, ui->orderSongsListView);
    setSongModelToView(favoriteSongs, ui->favoriteSongsListView);
    setSongModelToView(historySongs, ui->playHistoriesListView);

    QString vScrollBarSS("QScrollBar:vertical{"        //垂直滑块整体
                         "background: transparent;"  //背景色
                         "padding-top:0px;"    //上预留位置（放置向上箭头）
                         "padding-bottom:0px;" //下预留位置（放置向下箭头）
                         "padding-left:3px;"    //左预留位置（美观）
                         "padding-right:3px;"   //右预留位置（美观）
                         "border-left:1px solid #d7d7d7;}"//左分割线
                         "QScrollBar::handle:vertical{"//滑块样式
                         "background:#dbdbdb;"  //滑块颜色
                         "border-radius:4px;"   //边角圆润
                         "min-height:40px;}"    //滑块最小高度
                         "QScrollBar::handle:vertical:hover{"//鼠标触及滑块样式
                         "background:#d0d0d0;}" //滑块颜色
                         "QScrollBar::add-line:vertical{"//向下箭头样式
                         "background:transparent;}"
                         "QScrollBar::sub-line:vertical{"//向上箭头样式
                         "background:transparent;}");
    QString hScrollBarSS("QScrollBar:horizontal{"
                          "background:transparent;"
                          "padding-top:3px;"
                          "padding-bottom:3px;"
                          "padding-left:0px;"
                          "padding-right:0px;}"
                          "QScrollBar::handle:horizontal{"
                          "background:#dbdbdb;"
                          "border-radius:2px;"
                          "min-width:40px;}"
                          "QScrollBar::handle:horizontal:hover{"
                          "background:#d0d0d0;}"
                          "QScrollBar::add-line:horizontal{"
                          "background: transparent;}"
                          "QScrollBar::sub-line:horizontal{"
                          "background:transparent;}");
    ui->orderSongsListView->verticalScrollBar()->setStyleSheet(vScrollBarSS);
    ui->orderSongsListView->horizontalScrollBar()->setStyleSheet(hScrollBarSS);
    ui->favoriteSongsListView->verticalScrollBar()->setStyleSheet(vScrollBarSS);
    ui->favoriteSongsListView->horizontalScrollBar()->setStyleSheet(hScrollBarSS);
    ui->playHistoriesListView->verticalScrollBar()->setStyleSheet(vScrollBarSS);
    ui->playHistoriesListView->horizontalScrollBar()->setStyleSheet(hScrollBarSS);

    connect(ui->searchResultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortSearchResult(int)));

    searchMusic("哈哈哈");
}

OrderPlayerWindow::~OrderPlayerWindow()
{
    delete ui;
}


void OrderPlayerWindow::on_searchEdit_returnPressed()
{
    QString text = ui->searchEdit->text();
    searchMusic(text);
}

void OrderPlayerWindow::on_searchButton_clicked()
{
    on_searchEdit_returnPressed();
}

/**
 * 搜索音乐
 */
void OrderPlayerWindow::searchMusic(QString key)
{
    if (key.trimmed().isEmpty())
        return ;
    QString url = "http://iwxyi.com:3000/search?keywords=" + key.toUtf8().toPercentEncoding();
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest* request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request->setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36");
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply){
        QByteArray data = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(data, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << error.errorString();
            return ;
        }
        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200)
        {
            qDebug() << ("返回结果不为200：") << json.value("message").toString();
            return ;
        }

        QJsonArray songs = json.value("result").toObject().value("songs").toArray();
        searchResultSongs.clear();
        foreach (QJsonValue val, songs)
        {
            searchResultSongs << Song::fromJson(val.toObject());
        }

        setSearchResultTable(searchResultSongs);
    });
    manager->get(*request);
}

void OrderPlayerWindow::setSearchResultTable(QList<Song> songs)
{
    QTableWidget* table = ui->searchResultTable;
    enum {
        titleCol,
        artistCol,
        albumCol,
        durationCol,
        markCol
    };

    QFontMetrics fm(font());
    int fw = fm.horizontalAdvance("哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈");
    auto createItem = [=](QString s){
        QTableWidgetItem *item = new QTableWidgetItem();
        if (s.length() > 16 && fm.horizontalAdvance(s) > fw)
        {
            item->setToolTip(s);
            s = s.left(15) + "...";
        }
        item->setText(s);
        return item;
    };

    table->setRowCount(songs.size());
    for (int row = 0; row < songs.size(); row++)
    {
        Song song = songs.at(row);
        table->setItem(row, titleCol, createItem(song.name));
        table->setItem(row, artistCol, createItem(song.artistNames));
        table->setItem(row, albumCol, createItem(song.album.name));
        QString dur = QString("%1:%2").arg(song.duration/1000 / 60, 2, 10, QLatin1Char('0'))
                .arg(song.duration/1000 % 60, 2, 10, QLatin1Char('0'));
        table->setItem(row, durationCol, createItem(dur));
        table->setItem(row, markCol, createItem(snum(song.mark)));
    }
}

void OrderPlayerWindow::addFavorite(SongList songs)
{
    foreach (auto song, songs)
    {
        if (favoriteSongs.contains(song))
            continue;
        qDebug() << "添加收藏：" << song.simpleString();
    }
    saveSongList("music/favorite", favoriteSongs);
    setSongModelToView(favoriteSongs, ui->favoriteSongsListView);
}

void OrderPlayerWindow::removeFavorite(SongList songs)
{
    foreach (Song song, songs)
    {
        favoriteSongs.removeOne(song);
        qDebug() << "取消收藏：" << song.simpleString();
    }
    saveSongList("music/favorite", favoriteSongs);
    setSongModelToView(favoriteSongs, ui->favoriteSongsListView);
}

void OrderPlayerWindow::saveSongList(QString key, SongList songs)
{
    QJsonArray array;
    foreach (Song song, songs)
        array.append(song.toJson());
    settings.setValue(key, array);
}

void OrderPlayerWindow::restoreSongList(QString key, SongList &songs)
{
    QJsonArray array = settings.value(key).toJsonArray();
    foreach (QJsonValue val, array)
        songs.append(Song::fromJson(val.toObject()));
}

void OrderPlayerWindow::setSongModelToView(const SongList &songs, QListView *listView)
{
    QStringList sl;
    foreach (Song song, songs)
    {
        sl << song.simpleString();
    }
    QAbstractItemModel *model = listView->model();
    if (model)
        delete model;
    model = new QStringListModel(sl);
    listView->setModel(model);
}

bool OrderPlayerWindow::isSongDownloaded(Song song)
{
    return QFileInfo(musicsFileDir.absoluteFilePath(snum(song.id) + ".mp3")).exists();
}

void OrderPlayerWindow::showEvent(QShowEvent *)
{
    restoreGeometry(settings.value("orderplayerwindow/geometry").toByteArray());
    restoreState(settings.value("orderplayerwindow/state").toByteArray());
}

void OrderPlayerWindow::closeEvent(QCloseEvent *)
{
    settings.setValue("orderplayerwindow/geometry", this->saveGeometry());
    settings.setValue("orderplayerwindow/state", this->saveState());
}

void OrderPlayerWindow::on_searchResultTable_cellActivated(int row, int)
{
    Song song = searchResultSongs.at(row);

}

void OrderPlayerWindow::on_searchResultTable_customContextMenuRequested(const QPoint &)
{
    auto items = ui->searchResultTable->selectedItems();
    QList<Song> songs;
    foreach (auto item, items)
    {
        int row = ui->searchResultTable->row(item);
        int col = ui->searchResultTable->column(item);
        if (col != 0)
            continue;
        songs.append(searchResultSongs.at(row));
    }
    int row = ui->searchResultTable->currentRow();
    Song currentSong;
    if (row > -1)
        currentSong = searchResultSongs.at(row);

    FacileMenu* menu = new FacileMenu(this);
    menu->addAction("立即播放", [=]{
        startPlaySong(currentSong);
    })->disable(songs.size() != 1);

    menu->addAction("下一首播放", [=]{
        appendNextSongs(songs);
    })->disable(!currentSong.isValid());

    menu->addAction("添加到播放列表", [=]{
        appendOrderSongs(songs);
    })->disable(!currentSong.isValid());

    menu->split()->addAction("收藏", [=]{
        if (!favoriteSongs.contains(currentSong))
            addFavorite(songs);
        else
            removeFavorite(songs);
    })->disable(!currentSong.isValid())
            ->text(favoriteSongs.contains(currentSong), "从收藏中移除", "添加到收藏");

    menu->exec();
}

void OrderPlayerWindow::startPlaySong(Song song)
{
    if (isSongDownloaded(song))
    {
        playLocalSong(song);
    }
    else
    {
        playAfterDownloaded = song;
        downloadSong(song);
    }
}

void OrderPlayerWindow::appendOrderSongs(SongList songs)
{
    foreach (Song song, songs)
    {
        if (orderSongs.contains(song))
            continue;
        orderSongs.append(song);
        addDownloadSong(song);
    }
    saveSongList("music/order", orderSongs);
    setSongModelToView(orderSongs, ui->orderSongsListView);
    downloadNext();
}

void OrderPlayerWindow::appendNextSongs(SongList songs)
{
    foreach (Song song, songs)
    {
        if (orderSongs.contains(song))
            orderSongs.removeOne(song);
        orderSongs.insert(0, song);
        addDownloadSong(song);
    }
    saveSongList("music/order", orderSongs);
    setSongModelToView(orderSongs, ui->orderSongsListView);
    downloadNext();
}

/**
 * 立刻开始播放音乐
 */
void OrderPlayerWindow::playLocalSong(Song song)
{
    if (!isSongDownloaded(song))
    {
        qDebug() << "error: 未下载歌曲" << song.simpleString() << "开始下载";
        playAfterDownloaded = song;
        downloadSong(song);
        return ;
    }

    // 开始播放

}

void OrderPlayerWindow::addDownloadSong(Song song)
{
    if (isSongDownloaded(song) || toDownloadSongs.contains(song) || downloadingSong == song)
        return ;
    toDownloadSongs.append(song);
}

void OrderPlayerWindow::downloadNext()
{
    if (downloadingSong.isValid() || !toDownloadSongs.size())
        return ;
    Song song = toDownloadSongs.takeFirst();
    if (!song.isValid())
        return downloadNext();

    downloadSong(song);
}

/**
 * 下载音乐
 */
void OrderPlayerWindow::downloadSong(Song song)
{
    if (isSongDownloaded(song))
        return ;
    downloadingSong = song;
    QString url = API_DOMAIN + "/song/url?id=" + snum(song.id);
    qDebug() << "获取歌曲信息：" << song.simpleString();
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest* request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request->setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36");
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply){
        QByteArray baData = reply->readAll();
        qDebug() << baData;
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(baData, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << error.errorString();
            return ;
        }
        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200)
        {
            qDebug() << ("返回结果不为200：") << json.value("message").toString();
            return ;
        }

        QJsonArray array = json.value("data").toArray();
        if (!array.size())
        {
            qDebug() << "未找到歌曲：" << song.simpleString();
            downloadingSong = Song();
            downloadNext();
            return ;
        }

        json = array.first().toObject();
        QString url = JVAL_STR(url);
        int br = JVAL_INT(br); // 比率320000
        int size = JVAL_INT(size);
        QString type = JVAL_STR(type); // mp3
        QString encodeType = JVAL_STR(encodeType); // mp3
        qDebug() << "    信息：" << br << size << type << url;

        QNetworkAccessManager manager;
        QEventLoop loop;
        QNetworkReply *reply1 = manager.get(QNetworkRequest(QUrl(url)));
        //请求结束并下载完成后，退出子事件循环
        connect(reply1, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        //开启子事件循环
        loop.exec();
        QByteArray baData1 = reply1->readAll();
        qDebug() << "下载结束：" << baData1.size();

        QFile file(musicsFileDir.absoluteFilePath(snum(song.id)+".mp3"));
        file.open(QIODevice::WriteOnly);
        file.write(baData1);
        file.flush();
        file.close();

        downloadingSong = Song();
        downloadNext();
    });
    manager->get(*request);
}

void OrderPlayerWindow::on_listTabWidget_currentChanged(int index)
{
    settings.setValue("orderplayerwindow/tabIndex", index);
}

void OrderPlayerWindow::sortSearchResult(int col)
{

}
