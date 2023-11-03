#include "SigilOfBaphomet.h"
#include "player.h"

void SigilOfBaphomet::Start(Player* owner, Player* enemy)
{
    if(TimeToEndCooldown > 0)
        return;

    TimeToEndCooldown = Cooldown;

    this->Enemy = enemy;
    this->Owner = owner;

    if(Enemy)
        Enemy->AddHealth(-DamageEnemy);

    if (Owner) {
        Owner->AddHealth(-DamageOwner);
        float _buf = Owner->GetAttackSpeed();
        Owner->SetAttackSpeed(_buf + BonusAttackSpeed);
        _buf = Owner->GetDamage();
        Owner->SetDamage(_buf + BonusDamage);
    }
    isActive = true;
}

void SigilOfBaphomet::Tick(float DeltaTime)
{
    if(TimeToEndCooldown > 0)
        TimeToEndCooldown -= DeltaTime;

    if (isActive) {
        BonusTime -= DeltaTime;

        if (BonusTime <= 0.0f) {
            float _buf = Owner->GetDamage();
            Owner->SetDamage(_buf - BonusDamage);
            _buf = Owner->GetAttackSpeed();
            Owner->SetAttackSpeed(_buf - BonusAttackSpeed);
            isActive = false;
        }
    }
}
