#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QNetworkReply>
#include <QJsonDocument>
#include "twittermoderationmodel.h"
#include "progressdelegate.h"
#include "OAuth2Qt/src/o1twitter.h"
#include "OAuth2Qt/src/o1requestor.h"
#include <QFile>
#include <QSettings>
#include <QTcpSocket>
#include <QItemSelection>
#include <QWebEngineView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void newTweet(QJsonDocument*);

public slots:
    void onLinkedChanged();
    void onLinkingFailed();
    void onLinkingSucceeded();
    void onOpenBrowser(const QUrl &url);
    void onCloseBrowser();

    void onDisconnect();

    void makeRequest();
    void readFeed(qint64,qint64);
    void parseFeed();

    void reTweet();
    void reTweet(QModelIndex);
    void discardTweet(QPoint);
    void discardTweet(QModelIndex);
    void reTweetResponse();

    void selectionCell();
    void selectionCell(QModelIndex);
    void selectionCell(QItemSelection, QItemSelection);
    void selectionCellUrl(QUrl);
    void openHelp();

    void applySearch();



    void start();
    void startTransfer(QModelIndex);

    void setTwitterWallIp();
private:
    Ui::MainWindow *ui;

    O1Twitter *o1;
    QNetworkAccessManager* manager;
    QNetworkReply *reply;
    QString rawResponse;
    QString parsedResponse;
    int responseTweetSize;

    TwitterModerationModel* TwitterModel;
    ProgressDelegate* progressDelegate;

    QWebEngineView *webView;

    QFile* LogFile;
    QSettings settings;

    QTcpSocket client;
};

#endif // MAINWINDOW_H
