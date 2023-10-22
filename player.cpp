#include "player.h"

void Player::GetDataFromSocket() {
    QByteArray data = socket->readAll();
    qDebug() << "Data received from " << socket->peerAddress() << " is " << QString::fromUtf8(data);

    ParseInput(data);
}

void Player::SetAcceleration(const int& a)
{
    if(a >= 0)
        acceleration = a;
}

void Player::ParseInput(QByteArray data)
{
    QString dataString = QString::fromUtf8(data);

    for (auto a : dataString) {
        if (a == 'a')
            AddCommand(Commands::MoveLeft);
        else if (a == 'd')
            AddCommand(Commands::MoveRight);
        else if (a == 'j')
            AddCommand(Commands::Jump);
        else
            qDebug() << "Unkown command " << a;
    }
    MovementByCommands();
}

void Player::SetJumpAcceleration(const int& a)
{
    if(a >= 0)
        jumpAcceleration = a;
}

void Player::PlayerDisconnected()
{
    qDebug() << "IP address " << socket->peerAddress() << " disconnected";
    socket->deleteLater();

    disconnect(socket, SIGNAL(readyRead()), this, SLOT(GetDataFromSocket()));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(PlayerDisconnected()));

    socket = nullptr;
    emit LostConnection();
}

void Player::SetSocket(QTcpSocket* socket)
{
    if (!socket)
        return;
    this->socket = socket;

    connect(socket, SIGNAL(readyRead()), this, SLOT(GetDataFromSocket()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(PlayerDisconnected()));
}

QTcpSocket* Player::GetSocket() const
{
    return this->socket;
}

Player::Player(QObject* parent)
    : QObject{ parent }
{
    this->setParent(parent);
}

void Player::AddCommand(Commands cmd)
{
    commandList.append(cmd);
}

void Player::MovementByCommands()
{
    for (auto a : commandList) {
        switch (a)
        {
        case Commands::MoveLeft:
            if ((VelocityVector + FVector(acceleration)).Size() < maxSpeed)
                VelocityVector += FVector(acceleration);
            break;
        case Commands::MoveRight:
            if((VelocityVector + FVector(-acceleration)).Size() < maxSpeed)
                VelocityVector += FVector(-acceleration);
            break;
        case Commands::Jump:
            if (!isJumping) {
                VelocityVector += FVector(0, jumpAcceleration);
                isJumping = true;
            }
            break;
        default:
            break;
        }
    }
    ChangeLocation();
}

void Player::SetMaxSpeed(const int& a)
{
    if(a >= 0)
        this->maxSpeed = 0;
}

void Player::ChangeLocation()
{
    Location += VelocityVector;

    emit ChangedLocation();
}

FVector Player::GetCoordinates() const
{
    return Location;
}
