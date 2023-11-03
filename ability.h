#include "Actor.h"

class Player;

class Ability :
    public Actor
{
    Q_OBJECT
protected:
    float Cooldown;

public:
    bool isActive = false;

    Ability(QObject* parent = nullptr);
    virtual void Start(Player* owner, Player* enemy);
    virtual void Tick(float DeltaTime) override;
};

