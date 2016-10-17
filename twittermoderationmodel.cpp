#include "twittermoderationmodel.h"
#include "jsonparser.h"
#include <QDebug>

TwitterModerationModel::TwitterModerationModel(int tempo, QObject *parent) :
    QAbstractTableModel(parent)
{
    mRootItem = new TwitterModerationItem();
    mHeaders << "Time" << "User" << "Message";

    mTime = tempo;
    mTimer = new QTimer(this);
    mTimer->setInterval(1000);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(updateTimer()));
}

TwitterModerationModel::~TwitterModerationModel()
{
    delete mRootItem;
}

QModelIndex TwitterModerationModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TwitterModerationItem* parentItem;
    parentItem = getItem(parent);

    TwitterModerationItem* childItem = parentItem->child(row);
    if (childItem)
       return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

int TwitterModerationModel::rowCount(const QModelIndex &parent) const
{
    return getItem(parent)->childCount();
}

int TwitterModerationModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant TwitterModerationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    TwitterModerationItem* item = getItem(index);
    if (role == Qt::DisplayRole) {
        int column = index.column();
        QVariant dataReturn;
        switch (column) {
        case 0:
            dataReturn = item->tempo();
            break;
        case 1:
            dataReturn = JsonParser(item->getTweet()).parseJson("user/screen_name");
            break;
        case 2:
            dataReturn = JsonParser(item->getTweet()).parseJson("text");
            break;
        default:
            QJsonDocument* JsonDoc = item->getTweet();
            QByteArray JsonData = JsonDoc->toJson();
            dataReturn = QVariant(JsonData);
            break;
        }
        return dataReturn;
    }
    else if (role == Qt::UserRole)
    {
        QJsonDocument* JsonDoc = item->getTweet();
        QByteArray JsonData = JsonDoc->toJson();
        return QVariant(JsonData);
    }
    return QVariant();
}

QVariant TwitterModerationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {

        return mHeaders.value(section);
    }
    else
        return section;
}

QString TwitterModerationModel::getId(QModelIndex index)
{
    TwitterModerationItem* item = getItem(index);
    QJsonDocument* JsonDoc = item->getTweet();
    JsonParser parser(JsonDoc);
    QString Id = parser.parseJson("id_str");
    return Id;
}

QJsonDocument* TwitterModerationModel::getJsonDocument(QModelIndex index)
{
    TwitterModerationItem* item = getItem(index);
    QJsonDocument* JsonDoc = item->getTweet();
    return JsonDoc;
}

bool TwitterModerationModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool success;

    beginRemoveRows(parent, row, row + count - 1);
    success = mRootItem->removeChildren(row, count);
    endRemoveRows();
    return success;
}

void TwitterModerationModel::appendTweet(QJsonDocument* Tweet)
{
    beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());
    mRootItem->appendChild(Tweet, mTime);
    endInsertRows();
}

void TwitterModerationModel::deleteTweet(QModelIndex Tweet)
{
    removeRows(Tweet.row(), 1, Tweet.parent());

//    beginRemoveRows(Tweet.parent(), Tweet.row() - 1, Tweet.row() -1);
//    TwitterModerationItem* parentItem = getItem(Tweet.parent());
//    parentItem->removeChildren(Tweet.row() - 1, 1);
//    endRemoveRows();

}

void TwitterModerationModel::updateTimer()
{
    for(int i = 0; i < mRootItem->childCount(); i++)
    {
        TwitterModerationItem* TweetItem = mRootItem->child(i);
        TweetItem->setTempo(TweetItem->tempo() - 1);
    }
    QModelIndex topLeft = createIndex(0,0);
    QModelIndex bottomLeft = createIndex(rowCount(), 0);
    emit dataChanged(topLeft, bottomLeft);
}

void TwitterModerationModel::changeTimer(int newTime)
{

    for(int i = 0; i < mRootItem->childCount(); i++)
    {
        TwitterModerationItem* Tweet = mRootItem->child(i);
        Tweet->setTempo(Tweet->tempo() + (newTime - mTime));
    }
    mTime = newTime;
}

void TwitterModerationModel::resetTimer()
{
    for(int i = 0; i < mRootItem->childCount(); i++)
    {
        TwitterModerationItem* Tweet = mRootItem->child(i);
        Tweet->setTempo(mTime);
    }
}

void TwitterModerationModel::triggerTimer(bool trigger)
{
    if (trigger)
    {
        mTimer->start();
    }
    else
    {
        mTimer->stop();
    }
}

void TwitterModerationModel::clear()
{
    removeRows(0, rowCount(), QModelIndex());
//    qDebug() << "before : " << rowCount();
//    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
//    qDebug() << removeRows(0, rowCount(), QModelIndex());
//    endRemoveRows();
//    qDebug() << "after : " << rowCount();
}

TwitterModerationItem* TwitterModerationModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TwitterModerationItem *item = static_cast<TwitterModerationItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return mRootItem;
}

