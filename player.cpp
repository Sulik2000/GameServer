#include "player.h"

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
