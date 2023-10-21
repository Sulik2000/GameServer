#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QList>
#include <qtcpsocket.h>

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
    int X = 0, Y = 0;
    QTcpSocket* socket = nullptr;

protected slots:
    void GetDataFromSocket();
    void PlayerDisconnected();
public:
    void SetSocket(QTcpSocket* socket);
    QTcpSocket* GetSocket() const;
    explicit Player(QObject* parent = nullptr);
    void AddCommand(Commands cmd);
    void SetLookSide(LookSide side);
    QPair<int, int> GetCoordinates() const;
signals:

};

#endif // PLAYER_H
