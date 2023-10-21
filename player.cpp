#include "player.h"

void Player::GetDataFromSocket() {
    QByteArray data = socket->readAll();
    qDebug() << data.data();
}

void Player::PlayerDisconnected()
{
    qDebug() << "IP address " << socket->peerAddress() << " disconnected";
    socket->deleteLater();

    disconnect(socket, SIGNAL(readyRead()), this, SLOT(GetDataFromSocket()));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(PlayerDisconnected()));

    socket = nullptr;
}

void Player::SetSocket(QTcpSocket* socket)
{
    if(!socket)
        return;
    this->socket = socket;

    connect(socket, SIGNAL(readyRead()), this, SLOT(GetDataFromSocket()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(PlayerDisconnected()));
}

QTcpSocket* Player::GetSocket() const
{
    return this->socket;
}

Player::Player(QObject *parent)
    : QObject{parent}
{
    this->setParent(parent);
}

void Player::AddCommand(Commands cmd)
{
    commandList.append(cmd);
}

void Player::SetLookSide(LookSide side)
{
    lookSide = side;
}

QPair<int, int> Player::GetCoordinates() const
{
    return QPair<int, int>(X, Y);
}
