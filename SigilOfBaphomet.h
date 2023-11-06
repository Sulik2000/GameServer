#pragma once
#include "ability.h"

class Player;

class SigilOfBaphomet :
    public Ability
{
    Player* Owner, * Enemy;
    float DamageOwner = 200.0f, DamageEnemy = 100.0f;
    float BonusAttackSpeed = 50, BonusDamage = 50;
    float BonusTime = 3.0f; // In seconds

    float TimeToEndCooldown = 0.0f;
protected:
    float Cooldown = 30.0f, ActiveTime = 7.0f;

    virtual bool Start(Player* owner, Player* enemy) override;
    virtual void Tick(float DeltaTime) override;
};

