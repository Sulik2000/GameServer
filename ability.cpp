#include "ability.h"

Ability::Ability(QObject* parent)
{
    this->setParent(parent);
}

void Ability::Tick(float DeltaTime)
{
}

bool Ability::Start(Player* owner, Player* enemy)
{
    return true;
}
