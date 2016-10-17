#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_help.h"

#include <QDesktopServices>
#include <QMessageBox>
#include "QJsonDocument"
#include "progressdelegate.h"
#include "jsonparser.h"
#include <QSettings>
#include <QHostAddress>
#include "twitterwalldialog.h"

#include <QWebEngineView>

//TODO : remove qDebug messages

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Twitter Search
    ui->TwitterSearch->setText(settings.value("hashtag", "#Twitter").toString());
    connect(ui->TwitterSearchButton, SIGNAL(clicked()), this, SLOT(applySearch()));


    //WebView Dock
    webView = new QWebEngineView(ui->dockWidgetContents);
    ui->dockWidgetContents->layout()->addWidget(webView);
    webView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(ui->actionShow_Content_Panel, SIGNAL(triggered(bool)), ui->dockWidget, SLOT(setVisible(bool)));
    connect(ui->dockWidget, SIGNAL(visibilityChanged(bool)), ui->actionShow_Content_Panel, SLOT(setChecked(bool)));

    //LogFile
    LogFile = new QFile("LogFile");
    LogFile->open(QIODevice::WriteOnly);

    //Stream Size init
    responseTweetSize = 0;

    //Twitter Feed
    ui->TwitterFeed->setSelectionMode(QAbstractItemView::SingleSelection);
    TwitterModel = new TwitterModerationModel(ui->AutoValidationTempo->value(), this);
    connect(ui->actionClear_Tweets, SIGNAL(triggered()), TwitterModel, SLOT(clear()));
    ui->TwitterFeed->horizontalHeader()->setStretchLastSection(true);
    ui->TwitterFeed->horizontalHeader()->setSectionsClickable(false);
    ui->TwitterFeed->verticalHeader()->hide();
    ui->TwitterFeed->setModel(TwitterModel);


    //Retweet
    //connect(ui->ReTweet, SIGNAL(clicked()), this, SLOT(reTweet()));


    //Tempo Setup
    if (settings.value("AutoTempo", "0") == "2")
    {
        ui->AutoValidation->setChecked(true);
        TwitterModel->triggerTimer(true);
    }
    else
        ui->AutoValidation->setChecked(false);
    if (settings.contains("AutoTempoSlider"))
    {
        QVariant tempo = settings.value("AutoTempoSlider", ui->AutoValidationTempo->value());
        ui->AutoValidationTempo->setValue(tempo.toInt());
        ui->AutoValidationTempoLabel->setText(tempo.toString());
    }
    progressDelegate = new ProgressDelegate(ui->AutoValidationTempo->value());
    connect(ui->AutoValidationTempo, SIGNAL(valueChanged(int)), progressDelegate, SLOT(changeTimer(int)));
    connect(ui->AutoValidationTempo, SIGNAL(valueChanged(int)), TwitterModel, SLOT(changeTimer(int)));
    connect(ui->AutoValidation, SIGNAL(toggled(bool)), TwitterModel, SLOT(triggerTimer(bool)));
    connect(ui->AutoValidationReset, SIGNAL(clicked()), TwitterModel, SLOT(resetTimer()));
    connect(progressDelegate, SIGNAL(timeOut(QModelIndex)), this, SLOT(startTransfer(QModelIndex)));
    ui->TwitterFeed->setItemDelegateForColumn(0, progressDelegate);


    //QTableView Moderation
    ui->TwitterFeed->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->TwitterFeed, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(startTransfer(QModelIndex)));
    connect(ui->TwitterFeed, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(discardTweet(QPoint)));
    connect(ui->TwitterFeed, SIGNAL(clicked(QModelIndex)), this, SLOT(selectionCell(QModelIndex)));
    connect(ui->TwitterFeed->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionCell(QItemSelection,QItemSelection)));

    //QWebView
    //webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(webView, SIGNAL(linkClicked(QUrl)), this, SLOT(selectionCellUrl(QUrl)));
    connect(ui->ResetTweetView, SIGNAL(clicked()), this, SLOT(selectionCell()));

    //Help Dialog
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(openHelp()));

    //TwitterWall Connection
//    start();
//    connect(&client, SIGNAL(disconnected()), this, SLOT(start()));
    connect(ui->actionSet_TwitterWall_Ip, SIGNAL(triggered()), this, SLOT(setTwitterWallIp()));

    //Twitter OAuth Stream
    o1 = new O1Twitter(this);
    o1->setClientId("ihTu6B1fLtQBA8SMnLFQgu8gH");
    o1->setClientSecret("TClVDJhVEjfWInuNOXrhG1EROxqjNBHulJV1hBU7v3I5YPx7vK");
    o1->setLocalPort(8888);
    connect(o1, SIGNAL(linkedChanged()), this, SLOT(onLinkedChanged()));
    connect(o1, SIGNAL(linkingFailed()), this, SLOT(onLinkingFailed()));
    connect(o1, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
    connect(o1, SIGNAL(openBrowser(QUrl)), this, SLOT(onOpenBrowser(QUrl)));
    connect(o1, SIGNAL(closeBrowser()), this, SLOT(onCloseBrowser()));

    //Twitter User Disconnect
    connect(ui->actionDisconnect_User, SIGNAL(triggered()), this, SLOT(onDisconnect()));


    o1->link();

    manager = new QNetworkAccessManager(this);

    if (o1->linked())
        makeRequest();


}

MainWindow::~MainWindow()
{
    settings.setValue("AutoTempo", ui->AutoValidation->checkState());
    settings.setValue("AutoTempoSlider", ui->AutoValidationTempo->value());
    LogFile->close();
    delete ui;
}

void MainWindow::onLinkedChanged() {
    // Linking (login) state has changed.
    // Use o1->linked() to get the actual state
    qDebug() << "onLinkedChanged";
    if (o1->linked())
        makeRequest();
}

void MainWindow::onLinkingFailed() {
    // Login has failed
    qDebug() << "onLinkingFailed";
}

void MainWindow::onLinkingSucceeded() {
    // Login has succeeded
    QVariantMap extraToken = o1->extraTokens();
    if (extraToken.isEmpty())
        return;
    if  (!extraToken.contains("screen_name"))
        return;
    QSettings settings;
    settings.setValue("screen_name", extraToken["screen_name"]);
}

void MainWindow::onOpenBrowser(const QUrl &url) {
    // Open a web browser or a web view with the given URL.
    // The user will interact with this browser window to
    // enter login name, password, and authorize your application
    // to access the Twitter account
    qDebug() << "Opening browser with url" << url.toString();
    QDesktopServices::openUrl(url);
}

void MainWindow::onCloseBrowser() {
    // Close the browser window opened in openBrowser()
    qDebug() << "onCloseBrowser";
}

void MainWindow::onDisconnect()
{
    foreach (QString key, settings.allKeys()) {
        if (key.contains("token"))
            settings.remove(key);
    }
    QMessageBox msgBox;
    msgBox.setText("Redémarrer l'application pour appliquer");
    msgBox.exec();
}

void MainWindow::makeRequest()
{
    O1Requestor* requestor = new O1Requestor(manager, o1, this);

    QList<O1RequestParameter> reqParams = QList<O1RequestParameter>();
    O1RequestParameter track = O1RequestParameter("track", ui->TwitterSearch->text().toLatin1());
    O1RequestParameter length = O1RequestParameter("delimited", "length");
    reqParams << length << track;

    QByteArray postData = O1::createQueryParams(reqParams);

    QUrl url = QUrl("https://stream.twitter.com/1.1/statuses/filter.json");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    reply = requestor->post(request, reqParams, postData);
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(readFeed(qint64,qint64)));
}


void MainWindow::readFeed(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);

    rawResponse.append(reply->readAll());

//    qDebug() << rawResponse;

    parseFeed();
}

void MainWindow::parseFeed()
{
    QStringList responseSplit = QString(rawResponse).split(QRegExp("[\r\n]"),QString::SkipEmptyParts);

    foreach (QString split, responseSplit) {
        if (responseTweetSize == 0)
        {
            responseTweetSize = split.toInt();
//            qDebug() << "Length = " << responseTweetSize;
        }
        else
        {
            parsedResponse.append(split);
            if (parsedResponse.size() >= responseTweetSize - 3)
            {
                QJsonDocument* response = new QJsonDocument(QJsonDocument::fromJson(parsedResponse.toLatin1()));
                if (!response->isNull())
                {
//                    qDebug() << response->toJson();
//                    qDebug() << "screen_name : " << JsonParser(response).parseJson("user/screen_name");
                    if (JsonParser(response).parseJson("user/screen_name") != settings.value("screen_name", ""))
                        TwitterModel->appendTweet(response);
                }
                responseTweetSize = 0;
                parsedResponse.clear();
            }
        }
    }
    rawResponse.clear();
}

void MainWindow::reTweet()
{
    QModelIndexList selected = ui->TwitterFeed->selectionModel()->selectedIndexes();
    if (selected.size())
        reTweet(selected[0]);
}

void MainWindow::reTweet(QModelIndex index)
{
    QString id = TwitterModel->getId(index);
    qDebug() << "ReTweetId : " << id;

    O1Requestor* reTweetor = new O1Requestor(manager, o1, this);
    QList<O1RequestParameter> reqParams = QList<O1RequestParameter>();
    QByteArray postData = O1::createQueryParams(reqParams);
    QUrl url = QUrl("https://api.twitter.com/1.1/statuses/retweet/" + id + ".json");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");


    QNetworkReply *replyReTweet = reTweetor->post(request, reqParams, postData);
    connect(replyReTweet, SIGNAL(finished()), this, SLOT(reTweetResponse()));


    TwitterModel->deleteTweet(index);
}

void MainWindow::discardTweet(QPoint cursor)
{
    QModelIndex cursorIndex = ui->TwitterFeed->indexAt(cursor);
    discardTweet(cursorIndex);
}

void MainWindow::discardTweet(QModelIndex Index)
{
    int row = Index.row();
    int col = Index.column();
    TwitterModel->deleteTweet(Index);
    ui->TwitterFeed->selectionModel()->select(ui->TwitterFeed->model()->index(row, col, QModelIndex()),  QItemSelectionModel::Select);
    //ui->TwitterFeed->setCurrentIndex(ui->TwitterFeed->model()->index(row, col, QModelIndex()));
}

void MainWindow::reTweetResponse()
{
    QNetworkReply *replyEmbed = qobject_cast<QNetworkReply *>(sender());
    if (replyEmbed->error() != QNetworkReply::NoError) {
        qDebug() << "ERROR:" << replyEmbed->errorString();
        qDebug() << "content:" << replyEmbed->readAll();
    } else {
        qDebug() << "reTweet Response!";
        QByteArray rawEmbedTweet = replyEmbed->readAll();
        qDebug() << rawEmbedTweet;
    }
}

void MainWindow::selectionCell()
{
    QModelIndexList selected = ui->TwitterFeed->selectionModel()->selectedIndexes();
    if (selected.size())
        selectionCell(selected[0]);
}

void MainWindow::selectionCell(QModelIndex index)
{
    QJsonDocument* JsonDoc = TwitterModel->getJsonDocument(index);
    QString screenName = JsonParser(JsonDoc).parseJson("user/screen_name");
    QString tweetId    = JsonParser(JsonDoc).parseJson("id_str");
    QUrl TweetUrl = QUrl("https://twitter.com/" + screenName + "/status/" + tweetId);
    webView->setUrl(TweetUrl);

    return;
}

void MainWindow::selectionCell(QItemSelection selected, QItemSelection deselected)
{
    Q_UNUSED(deselected);
    if (!selected.size())
        return;
    QModelIndex selection = selected.indexes()[0];
    selectionCell(selection);
}

void MainWindow::selectionCellUrl(QUrl url)
{
    qDebug() << url;
    webView->setUrl(url);
}

void MainWindow::openHelp()
{
    QDialog* helpDialog = new QDialog(this);
    Ui_HelpDialog helpUi;
    helpUi.setupUi(helpDialog);
    helpDialog->show();

}

void MainWindow::applySearch()
{
    QSettings settings;
    settings.setValue("hashtag", ui->TwitterSearch->text());
    QMessageBox msgBox;
    msgBox.setText("Redémarrer l'application pour appliquer");
    msgBox.exec();
}



void MainWindow::start()
{
    QHostAddress addr(settings.value("TwitterWallIp", "127.0.0.1").toString());
    client.connectToHost(addr, 8889);
}

void MainWindow::startTransfer(QModelIndex index)
{
    QHostAddress addr(settings.value("TwitterWallIp", "127.0.0.1").toString());
    QTcpSocket* tcpclient = new QTcpSocket(this);
    tcpclient->connectToHost(addr, 8889);
    tcpclient->waitForConnected();
    connect(tcpclient, SIGNAL(disconnected()), tcpclient, SLOT(deleteLater()));
    qDebug() << tcpclient->state();

    QJsonDocument* Json = TwitterModel->getJsonDocument(index);
    QByteArray JsonBytes = Json->toJson();

    QByteArray block;

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << (quint32)0;
    out << JsonBytes;
    out.device()->seek(0);
//    qDebug() << "quint32 >? " << (block.size() > sizeof(quint32)) << " " << block.size() << " " << sizeof(quint8);
//    qDebug() << numeric_limits<qint16>::max() << " " << numeric_limits<qint32>::max();
    out << (quint32)(block.size() - sizeof(quint32));

    tcpclient->write(block);
    tcpclient->disconnectFromHost();
    discardTweet(index);

//    int JsonSize = JsonBytes.size();
//    QByteArray JsonBytesSize = QByteArray::number(JsonSize);
//    QByteArray JsonWrite = QByteArray(JsonBytesSize + '\r' + '\n' + JsonBytes + '\r' + '\n');

//    int dataSize = JsonWrite.size();
//    int dataReceived = 0;

//    while (dataReceived < dataSize)
//    {
//        if (client.)
//        client.write(JsonWrite.left(1024), 1024);
//        JsonWrite = JsonWrite.right(JsonWrite.size() - 1024);
//    }
//    for(int i = 0; i < JsonSize; i = i + 1024)
//    {
//        if (client.state() == QTcpSocket::UnconnectedState)
//        {
//            qDebug() << "Not Connected to Wall";
//            return;
//        }
//        qDebug() << JsonWrite.left(1024);
//        client.write()
//                (JsonWrite.left(1024), 1024);

//        client.flush();

//        JsonWrite = JsonWrite.right(JsonWrite.size() - 1024);
//    client.flush();
//    }
//    client.write(block);
//    client.disconnectFromHost();
//    discardTweet(index);

}

void MainWindow::setTwitterWallIp()
{
    TwitterWallDialog TwitterDialog(this);
    TwitterDialog.setIp(settings.value("TwitterWallIp", "127.0.0.1").toString());
    if (TwitterDialog.exec() == QDialog::Accepted)
    {
        QString Ip = TwitterDialog.getIp();
        settings.setValue("TwitterWallIp", Ip);
        if (client.state() == QTcpSocket::ConnectingState)
        {
            client.abort();
        }
        if (client.state() == QTcpSocket::ConnectedState)
        {
            client.disconnectFromHost();
        }
        start();
    }
}

