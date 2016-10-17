#ifndef TWITTERMODERATIONITEM_H
#define TWITTERMODERATIONITEM_H

#include <QList>
#include <QJsonDocument>
#include <QTime>

class TwitterModerationItem
{
public:
    TwitterModerationItem(TwitterModerationItem *parent = 0, QJsonDocument* JsonTweet = 0, int tempo = 10);
    ~TwitterModerationItem();

    int childCount();
    void appendChild(QJsonDocument* Tweet, int tempoTime);
    bool removeChildren(int position, int rows);

    TwitterModerationItem *child(int row);

    QJsonDocument *getTweet() const;
    void setTweet(QJsonDocument *value);


    int tempo() const;
    void setTempo(int tempo);

private:
    TwitterModerationItem *mParent;
    QList<TwitterModerationItem*> mChilds;
    QJsonDocument* Tweet;

    int mTempo;
};

#endif // TWITTERMODERATIONITEM_H
