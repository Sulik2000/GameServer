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
#include <qlist.h>


#define TICKRATE 64.0

constexpr float DeltaTime = 1.0 / TICKRATE;

class Server : public QTcpServer
{
    Q_OBJECT

private:
    QJsonObject CreateBaseSettings();

    QJsonObject ParseDataFromFile();

    QList<Actor*> actors;

    QTimer TickTimer;

    bool isGameEnd = false;
protected:
    void OnEndGame(Player* player);

    std::array<Player*, 2> Players;

    QJsonObject GameData;

    void SetPlayerData(QJsonObject obj);

    QJsonDocument ParseToJsonPlayerInfo();

    void ParseGameData();
public:
    

    void AddNewActor(Actor* actor);
    
    virtual void Tick();

    Server(QObject* parent = nullptr);
    ~Server();

    // Function starts server
    void Start();

public slots:
    void SendInfoPlayerOne();
    void SendInfoPlayerTwo();

    void CastAbility(Player* owner, Ability* ability);

    // Function calculates attack of player
    void Attack(Player* owner);

    // Function accepts new connection and send signal "newConnection" with argument of new socket
    void AcceptNewConnection();

    void OnDiedPlayer(Player* player);
signals:
    void getConnection(QTcpSocket* socket);
};

#endif // SERVER_H
