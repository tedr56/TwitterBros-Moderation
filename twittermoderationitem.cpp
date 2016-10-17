#include "twittermoderationitem.h"

TwitterModerationItem::TwitterModerationItem(TwitterModerationItem *parent, QJsonDocument* JsonTweet, int tempo)
{
    mParent = parent;
    Tweet = JsonTweet;
    mTempo = tempo;
}

TwitterModerationItem::~TwitterModerationItem()
{
}

int TwitterModerationItem::childCount()
{
    return mChilds.count();
}

void TwitterModerationItem::appendChild(QJsonDocument * Tweet, int tempoTime)
{
    TwitterModerationItem* newTweet = new TwitterModerationItem(this, Tweet, tempoTime);
    mChilds.append(newTweet);
}

bool TwitterModerationItem::removeChildren(int position, int rows)
{
    if (position < 0 || position + rows > mChilds.size())
        return false;

//    int row = position;
    for (int row = position; row < position + rows; row++)
    {
        delete mChilds.takeAt(position);
    }

    return true;
}

TwitterModerationItem *TwitterModerationItem::child(int row)
{
    return mChilds.value(row);
}
QJsonDocument *TwitterModerationItem::getTweet() const
{
    return Tweet;
}

void TwitterModerationItem::setTweet(QJsonDocument *value)
{
    Tweet = value;
}
int TwitterModerationItem::tempo() const
{
    return mTempo;
}

void TwitterModerationItem::setTempo(int tempo)
{
    mTempo = tempo;
}



