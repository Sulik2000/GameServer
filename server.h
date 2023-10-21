#ifndef SERVER_H
#define SERVER_H

#define PORT 64378

#include "player.h"

#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QArrayData>
#include <array>
#include <QThread>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <thread>

class Server : public QTcpServer
{
    Q_OBJECT

protected:
    std::array<QTcpSocket*, 2> connections;
    std::array<Player*, 2> Players;
    std::array<QThread*, 2> Threads;
public:
    Server(QObject* parent = nullptr);
    ~Server();

    // Function starts server
    void Start();

public slots:
    // Function accepts new connection and send signal "newConnection" with argument of new socket
    void AcceptNewConnection();

signals:
    void getConnection(QTcpSocket* socket);
};

#endif // SERVER_H
