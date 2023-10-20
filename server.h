#ifndef SERVER_H
#define SERVER_H

#define PORT 34523
#define BUFFER_SIZE 100

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

    // Function processes data from client to parse commands into the server
    void ParseInput(QJsonObject object);
public:
    Server(QObject* parent = nullptr);
    ~Server();

    // Function starts server
    void Start();

public slots:
    // Function receives data from client's socket
    void GetDataFromSocket(QTcpSocket* socket);
    // Function accepts new connection and send signal "newConnection" with argument of new socket
    void AcceptNewConnection();

signals:
    void getConnection(QTcpSocket* socket);
};

#endif // SERVER_H
