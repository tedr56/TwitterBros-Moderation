#include "progressdelegate.h"

ProgressDelegate::ProgressDelegate( int timer, QObject *parent)
: QStyledItemDelegate(parent)
{
    mTimer = timer;
}


void ProgressDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0) {
        int progress = index.data().toInt();

        if (progress <= 0)
        {
            emit timeOut(index);
        }

        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = option.rect;
        progressBarOption.minimum = 0;
        progressBarOption.maximum = mTimer;
        progressBarOption.progress = progress;
        progressBarOption.text = QString::number(progress);
        progressBarOption.textVisible = true;

        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                           &progressBarOption, painter);
    } else
        QStyledItemDelegate::paint(painter, option, index);
}

void ProgressDelegate::changeTimer(int newTimer)
{
    mTimer = newTimer;
}
