#include "server.h"

Server::Server(QObject* parent)
{
    this->setParent(parent);

    Players[0] = new Player(this);
    Players[0]->SetMaxSpeed(100);
    Players[0]->SetAcceleration(5);
    Players[0]->SetJumpAcceleration(100);

    connect(Players[0], SIGNAL(ChangedLocation()), this, SLOT(SendLocationPlayers()));
    
    Players[1] = new Player(this);
    Players[1]->SetMaxSpeed(100);
    Players[1]->SetAcceleration(5);
    Players[1]->SetJumpAcceleration(100);

    connections[0] = nullptr;
    connections[1] = nullptr;

    connect(Players[1], SIGNAL(ChangedLocation()), this, SLOT(SendLocationPlayers()));
}

Server::~Server()
{
    close();

    for (auto a : Players)
        delete a;

    for (auto a : Threads)
        delete a;
}

void Server::Start()
{
    if (!listen(QHostAddress::Any, PORT)) {
        qDebug() << "Error: " << errorString();
        exit(1);
    }

    connect(this, SIGNAL(newConnection()), this, SLOT(AcceptNewConnection()));
}

void Server::SendLocationPlayers()
{
    auto socket1 = Players[0]->GetSocket();
    auto socket2 = Players[1]->GetSocket();

    QJsonObject jsonObj;

    jsonObj.insert("p1", Players[0]->GetCoordinates().GetString());
    jsonObj.insert("p2", Players[1]->GetCoordinates().GetString());

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    QByteArray data = jsonDoc.toJson();
    if(socket1)
        socket1->write(data);
    if(socket2)
        socket2->write(data);
}

void Server::AcceptNewConnection()
{
    auto newConnection = nextPendingConnection();
    if (!newConnection)
        return;

    qDebug() << "Accepted new connection " << newConnection->peerAddress().toString();

    if(!Players[0]->GetSocket())
        Players[0]->SetSocket(newConnection);
    else if(!Players[1]->GetSocket())
        Players[1]->SetSocket(newConnection);
    else {
        qDebug() << "Full server, cannot accept connection " << newConnection->peerAddress() << "; Disconnect socket...";
        newConnection->close();
        delete newConnection;
    }
}
