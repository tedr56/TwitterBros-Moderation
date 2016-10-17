#ifndef TWITTERMODERATIONMODEL_H
#define TWITTERMODERATIONMODEL_H

#include <QAbstractTableModel>
#include <QJsonDocument>
#include "twittermoderationitem.h"
#include <QTimer>

class TwitterModerationItem;

class TwitterModerationModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TwitterModerationModel(int tempo, QObject *parent = 0);
    ~TwitterModerationModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QString getId(QModelIndex index);
    QJsonDocument* getJsonDocument(QModelIndex index);
    bool removeRows(int row, int count, const QModelIndex &parent);
public slots:
    void appendTweet(QJsonDocument* Tweet);
    void deleteTweet(QModelIndex Tweet);
    void updateTimer();
    void changeTimer(int);
    void resetTimer();
    void triggerTimer(bool);
    void clear();

private:
    TwitterModerationItem * mRootItem;

    TwitterModerationItem* getItem(const QModelIndex &index) const;

    QStringList mHeaders;

    int mTime;
    QTimer* mTimer;
};

#endif // TWITTERMODERATIONMODEL_H
