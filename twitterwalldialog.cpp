#include "twitterwalldialog.h"
#include "ui_twitterwalldialog.h"

TwitterWallDialog::TwitterWallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TwitterWallDialog)
{
    ui->setupUi(this);
}

TwitterWallDialog::~TwitterWallDialog()
{
    delete ui;
}

QString TwitterWallDialog::getIp()
{
    return ui->TwitterWallIp->text();
}

void TwitterWallDialog::setIp(QString ip)
{
    ui->TwitterWallIp->setText(ip);
}
