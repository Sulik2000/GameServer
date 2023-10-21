#include "server.h"

Server::Server(QObject* parent)
{
    this->setParent(parent);

    Players[0] = new Player(this);
    Players[1] = new Player(this);
    connections[0] = nullptr;
    connections[1] = nullptr;
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
