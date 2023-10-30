#ifndef PLAYER_H
#define PLAYER_H
#define TICKRATE 128

#include <QObject>
#include <QList>
#include <qtcpsocket.h>
#include <qtimer.h>
#include "vector.h"
#include <qtimer.h>
#include <QQueue>

enum struct LookSide {
    Right,
    Left
};

enum struct Commands {
    MoveLeft,
    MoveRight,
    Jump,
    Attack
};

class Player : public QObject
{
    Q_OBJECT

protected:
    QQueue<Commands> commandList;
    LookSide lookSide = LookSide::Right;
    QTcpSocket* socket = nullptr;
    int acceleration = 0, jumpAcceleration = 0, maxSpeed = 0;
    FVector VelocityVector, Location, Gravity;
    int FloorHeight = 420;
    float ping = 0;
    float Health = 3000.0f;

    // Need to get ping of player
    void PingTimer();
protected slots:
    // This function gets data from client socket about input
    void GetDataFromSocket();

    // This function calls when client socket disconnected from server
    void PlayerDisconnected();

public:
    void ReceiveDamage(float Damage);

    void SetHealth(const float& health);

public:
    // This function is called when player pressed button of attack
    virtual void Attack();

    // Sets player gravity
    void SetGravity(FVector gravity);

    // Sets for player client socket where gets input from client
    void SetSocket(QTcpSocket* socket);

    // Sets floor height for player
    void SetFloorHeight(int height);

    // Get client's socket
    QTcpSocket* GetSocket() const;

    explicit Player(QObject* parent = nullptr);

    // Get location of player
    FVector GetLocation() const;

    // Set location from server
    void SetLocation(const FVector& coordinates);

    // Sets acceleration of movement of player
    void SetAcceleration(const int& a);

    // Sets acceleration of jump
    void SetJumpAcceleration(const int& a);

    // Sets max speed for the player
    void SetMaxSpeed(const int& a);

signals:
    // Emits when player press button of attack
    void MakeAttack(FVector PlayerLocation);

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

    quint64 beginTimePing = 0, endTimePing = 0;
};

#endif // PLAYER_H
