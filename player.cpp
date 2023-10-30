#include "player.h"
#include <QDateTime>

void Player::GetDataFromSocket() {
    QByteArray data = socket->readAll();
    qDebug() << "Data received from " << socket->peerAddress() << " is " << QString::fromUtf8(data);
    ParseInput(data);
}

void Player::SetFloorHeight(int height) {
    if (height >= 0)
        FloorHeight = height;
}

void Player::SetAcceleration(const int& a)
{
    if (a >= 0)
        acceleration = a;
}

void Player::SetGravity(FVector gravity)
{
    this->Gravity = gravity;
}

void Player::Attack()
{
    emit MakeAttack(this->Location);
}

void Player::ReceiveDamage(float Damage)
{
    if(Damage >= 0)
        this->Health -= Damage;
}

void Player::ParseInput(QByteArray data)
{
    QString dataString = QString::fromUtf8(data);

    for (auto a : dataString) {
        switch (a.toLatin1()) {
            case 'r':
                AddCommand(Commands::MoveRight);
                break;
            case 'l':
                AddCommand(Commands::MoveLeft);
                break;
            case 'j':
                AddCommand(Commands::Jump);
                break;
            case 'h':
                AddCommand(Commands::Attack);
            default:
                qDebug() << "Unknown command " << a;
                break;
        }
    }
    MovementByCommands();
}

void Player::SetJumpAcceleration(const int& a)
{
    if (a >= 0)
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
    beginTimePing = QDateTime::currentDateTime().toMSecsSinceEpoch();
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

void Player::SetLocation(const FVector& coordinates)
{
    this->Location = coordinates;
}

void Player::AddCommand(Commands cmd)
{
    commandList.append(cmd);
}

void Player::SetHealth(const float& health)
{
    if(Health > 0)
        this->Health = Health;
}

void Player::MovementByCommands()
{
    endTimePing = QDateTime::currentDateTime().toMSecsSinceEpoch();
    ping = float(endTimePing - beginTimePing) / 1000.0;
    while(commandList.size() > 0) {
        const auto a = commandList.dequeue();
        switch (a)
        {
        case Commands::MoveLeft:
            if ((VelocityVector + FVector(-acceleration)).Size() < maxSpeed)
                VelocityVector += FVector(-acceleration);
            break;
        case Commands::MoveRight:
            if ((VelocityVector + FVector(acceleration)).Size() < maxSpeed)
                VelocityVector += FVector(acceleration);
            break;
        case Commands::Jump:
            if (!isJumping) {
                VelocityVector += FVector(0, -jumpAcceleration);
                isJumping = true;
            }
            break;
        case Commands::Attack:
            emit MakeAttack(Location);
        default:
            break;
        }
    }
    ChangeLocation();
}

void Player::SetMaxSpeed(const int& a)
{
    if (a >= 0)
        this->maxSpeed = a;
}

void Player::ChangeLocation()
{
    if (Location.Y + (VelocityVector.Y * (ping)) > FloorHeight) {
        Location.Y = FloorHeight;
        VelocityVector.Y = 0;
        isJumping = false;
    }
    else
        Location.Y += VelocityVector.Y * ping;

    if(Location.X + VelocityVector.X * ping >= 0 && Location.X + VelocityVector.X <= 1280)
        Location.X += VelocityVector.X * ping;
    else if (Location.X + VelocityVector.X * ping > 1280) {
        Location.X = 1280;
        VelocityVector.X = 0;
    }
    else {
        Location.X = 0;
        VelocityVector.X = 0;
    }

    FVector _buf = Gravity;

    if (VelocityVector.X < 0)
        VelocityVector += _buf * ping;
    else if (VelocityVector.X > 0) {
        _buf.X *= -1;
        VelocityVector += _buf * ping;
    }

    emit ChangedLocation();
    beginTimePing = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

FVector Player::GetLocation() const
{
    return Location;
}
