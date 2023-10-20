#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QList>

enum struct LookSide{
    Right,
    Left
};

enum struct Commands{
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

public:
    explicit Player(QObject *parent = nullptr);
    void AddCommand(Commands cmd);
    void SetLookSide(LookSide side);
    QPair<int, int> GetCoordinates() const;
signals:

};

#endif // PLAYER_H
