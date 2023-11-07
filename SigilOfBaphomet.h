#pragma once
#include "ability.h"

class Player;

class SigilOfBaphomet :
    public Ability
{
    const float ActiveTime = 3.0f;
    Player* Owner, * Enemy;
    float DamageOwner = 200.0f, DamageEnemy = 100.0f;
    float BonusAttackSpeed = 2, BonusDamage = 50;
    float BonusTime = 0.0f; // In seconds

    float TimeToEndCooldown = 0.0f;
protected:
    float Cooldown = 30.0f;

    virtual bool Start(Player* owner, Player* enemy) override;
    virtual void Tick(float DeltaTime) override;
};

