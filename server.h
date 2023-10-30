#ifndef SERVER_H
#define SERVER_H

#define DEFAULT_PORT 64378

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
#include <QFile>
#include <qcoreapplication.h>

#include <thread>

class Server : public QTcpServer
{
    Q_OBJECT

private:
    QJsonObject CreateBaseSettings();

    QJsonObject ParseDataFromFile();

    float AttackRange{0.0f}, attackDmg{50.0f};
protected:
    std::array<Player*, 2> Players;

    QJsonObject GameData;

    void SetPlayerData(QJsonObject obj);

    QJsonDocument ParseToJsonPlayerLocation();

    void ParseGameData();
public:
    Server(QObject* parent = nullptr);
    ~Server();

    // Function starts server
    void Start();

public slots:
    // Function calculates attack of player
    void Attack(FVector PlayerLocation);

    // Function accepts new connection and send signal "newConnection" with argument of new socket
    void AcceptNewConnection();

    void SendLocationPlayers();

signals:
    void getConnection(QTcpSocket* socket);
};

#endif // SERVER_H
