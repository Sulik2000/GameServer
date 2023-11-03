#include "ability.h"

Ability::Ability(QObject* parent)
{
    this->setParent(parent);
}

void Ability::Tick(float DeltaTime)
{
}

void Ability::Start(Player* owner, Player* enemy)
{
}
