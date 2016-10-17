#ifndef PROGRESSDELEGATE_H
#define PROGRESSDELEGATE_H

#include <QStyledItemDelegate>
#include <QApplication>
#include <QTime>

class ProgressDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ProgressDelegate(int timer, QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
signals:
    void timeOut(QModelIndex) const;
public slots:
    void changeTimer(int);
private:
    int mTimer;

};

#endif // PROGRESSDELEGATE_H
