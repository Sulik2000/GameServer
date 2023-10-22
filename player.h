#ifndef PLAYER_H
#define PLAYER_H
#define TICKRATE 128

#include <QObject>
#include <QList>
#include <qtcpsocket.h>
#include <qtimer.h>
#include "vector.h"

enum struct LookSide {
    Right,
    Left
};

enum struct Commands {
    MoveLeft,
    MoveRight,
    Jump
};

class Player : public QObject
{
    Q_OBJECT

protected:
    QList<Commands> commandList;
    LookSide lookSide = LookSide::Right;
    QTcpSocket* socket = nullptr;
    // TODO: Make tickrate system
    double TickRate = 1000 / TICKRATE;
    int acceleration = 0, jumpAcceleration = 0, maxSpeed = 0;
    FVector VelocityVector, Location;
    
protected slots:
    // This function gets data from client socket about input
    void GetDataFromSocket();

    // This function calls when client socket disconnected from server
    void PlayerDisconnected();
public:
    // Sets for player client socket where gets input from client
    void SetSocket(QTcpSocket* socket);

    // Get client's socket
    QTcpSocket* GetSocket() const;

    explicit Player(QObject* parent = nullptr);

    // Get location of player
    FVector GetCoordinates() const;

    // Sets acceleration of movement of player
    void SetAcceleration(const int& a);

    // Sets acceleration of jump
    void SetJumpAcceleration(const int& a);

    // Sets max speed for the player
    void SetMaxSpeed(const int& a);

signals:
    // Emits every time when player change his location
    void ChangedLocation();

    // Emits when client disconnected from server
    void LostConnection();

private:
    bool isJumping = false;

    void ParseInput(QByteArray data);
    void AddCommand(Commands cmd);
    void MovementByCommands();
    void ChangeLocation();
};

#endif // PLAYER_H
