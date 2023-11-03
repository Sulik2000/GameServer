#include <qobject.h>
#include "vector.h"

class Actor :
    public QObject
{
    Q_OBJECT
protected:
    FVector Location = FVector(0, 0);
public:
    virtual void Tick(float DeltaTime);
    Actor(QObject* parent = nullptr);
};

