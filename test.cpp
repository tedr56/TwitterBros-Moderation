#include <QtCore>
#include <QTcpServer>
#include <QTcpSocket>

class Receiver: public QObject
{
    Q_OBJECT
public:
    Receiver(QTcpSocket *socket, QObject *p = 0):
        QObject(p),
        sock(socket),
        bytesExpected(0)
    {
        connect(socket, SIGNAL(readyRead()), SLOT(readyRead()));
    }

public slots:

    void readyRead()
    {
        if (bytesExpected == 0 && sock->bytesAvailable() >= sizeof(bytesExpected)) {
            sock->read((char *)&bytesExpected, sizeof(bytesExpected));
            qDebug() << "Expecting:" << &bytesExpected;
        }

        if (bytesExpected > 0 && sock->bytesAvailable() > 0) {
            QByteArray chunk = sock->read(qMin(bytesExpected, sock->bytesAvailable()));
            buffer += chunk;
            bytesExpected -= chunk.size();
            qDebug() << "Received chunk of:" << chunk.size();

            if (bytesExpected == 0) {
                qDebug() << "Received block of size:" << buffer.size();
                qDebug() << "Bytes left in socket:" << sock->bytesAvailable();
                sock->deleteLater();
                deleteLater();
            }
        }
    }

private:
    QTcpSocket *sock;
    qint64 bytesExpected;
    QByteArray buffer;
};

class Server: public QObject
{
    Q_OBJECT
public:
    Server(QObject *p = 0): QObject(p)
    {
        server = new QTcpServer(this);
        connect(server, SIGNAL(newConnection()), SLOT(handleConnection()));
        server->listen(QHostAddress::Any, 12345);
    }
public slots:
    void handleConnection()
    {
        while(server->hasPendingConnections())
            (void) new Receiver(server->nextPendingConnection(), this);
    }
private:
    QTcpServer *server;
};

int main(int argc, char **argv)
{
    QCoreApplication app (argc, argv);

    Server s;

    return app.exec();

}
#include "main.moc"







sender()
{
    //create a socket in serevr side
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();

    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));
    //code for reading the file
    QFile inputFile("path to file to be read");
    inputFile.open(QIODevice::ReadOnly);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)0;
    QByteArray qwerty=inputFile.readAll();


    block.append(qwerty);

    inputFile.close();
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    qint64 x=0;
    while(x<block.size())
    {
        qint64 y=clientConnection->write(block);
        x=x+y;
        qDebug()<<x;
    }

    clientConnection->disconnectFromHost();
}
