#ifndef TWITTERWALLDIALOG_H
#define TWITTERWALLDIALOG_H

#include <QDialog>

namespace Ui {
class TwitterWallDialog;
}

class TwitterWallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TwitterWallDialog(QWidget *parent = 0);
    ~TwitterWallDialog();
    QString getIp();
    void setIp(QString ip);
private:
    Ui::TwitterWallDialog *ui;
};

#endif // TWITTERWALLDIALOG_H
