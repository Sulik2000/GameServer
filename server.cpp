#include "server.h"

#define DEFAULT_MAX_SPEED 100
#define DEFAULT_ACCELERATION 10
#define DEFAULT_JUMP_ACCELERATION 100
#define DEFAULT_FLOOR_HEIGHT 420
#define DEFAULT_GRAVITY 10
#define DEFAULT_FRICTION 5
#define DEFAULT_PLAYER1_POSITION {0, 420}
#define DEFAULT_PLAYER2_POSITION {1280, 420}
#define DEFAULT_ATTACK_RANGE 100
#define DEFAULT_HEALTH 3000
#define DEFAULT_DAMAGE 10.0f

QJsonObject Server::CreateBaseSettings()
{
    QFile file(this);
    file.setFileName(QCoreApplication::applicationDirPath() + "/settings.json");
    if (!file.open(QIODeviceBase::WriteOnly)) {
        qDebug() << "Cannot create settings default file. Turning off server...";
        exit(1);
    }
    QJsonObject map, mapPlayer1, mapPlayer2;
    map.insert("maxSpeed", DEFAULT_MAX_SPEED);
    map.insert("acceleration", DEFAULT_ACCELERATION);
    map.insert("jumpAcceleration", DEFAULT_JUMP_ACCELERATION);
    map.insert("gravity", DEFAULT_GRAVITY);
    map.insert("friction", DEFAULT_FRICTION);
    map.insert("attackRange", DEFAULT_ATTACK_RANGE);
    map.insert("health", DEFAULT_HEALTH);
    map.insert("damage", DEFAULT_DAMAGE);

    mapPlayer1.insert("startPosition_X", QPair<int, int>DEFAULT_PLAYER1_POSITION.first);
    mapPlayer1.insert("startPosition_Y", QPair<int, int>DEFAULT_PLAYER1_POSITION.second);
    
    mapPlayer2.insert("startPosition_X", QPair<int, int>DEFAULT_PLAYER2_POSITION.first);
    mapPlayer2.insert("startPosition_Y", QPair<int, int>DEFAULT_PLAYER2_POSITION.second);

    QJsonObject obj;
    obj.insert("player", map);
    obj.insert("player1", mapPlayer1);
    obj.insert("player2", mapPlayer2);

    obj.insert("floorHeight", DEFAULT_FLOOR_HEIGHT);
    obj.insert("port", DEFAULT_PORT);

    QJsonDocument doc;
    doc.setObject(obj);
    QByteArray data = doc.toJson();
    file.write(data);
    file.close();
    return obj;
}

QJsonObject Server::ParseDataFromFile()
{
    QFile file(this);
    file.setFileName(QCoreApplication::applicationDirPath() + "/settings.json");
    file.open(QIODeviceBase::ReadOnly);
    return QJsonDocument::fromJson(file.readAll()).object();
}

void Server::Attack(FVector PlayerLocation){
    for (auto a : Players) {
        if ((a->GetLocation() + (PlayerLocation * -1)).Size() <= AttackRange && a->GetLocation() != PlayerLocation)
            a->ReceiveDamage(attackDmg);
    }
}

void Server::SetPlayerData(QJsonObject obj)
{
    Players[0] = new Player(this);

    Players[0]->SetMaxSpeed(obj.value("player").toObject().value("maxSpeed").toInt());

    Players[0]->SetAcceleration(obj.value("player").toObject().value("acceleration").toInt());

    Players[0]->SetJumpAcceleration(obj.value("player").toObject().value("jumpAcceleration").toInt());

    Players[0]->SetGravity(FVector(obj.value("player").toObject().value("friction").toInt(),
        obj.value("player").toObject().value("gravity").toInt()));

    Players[0]->SetLocation(FVector(obj.value("player1").toObject().value("startPosition_X").toInt(),
        obj.value("player1").toObject().value("startPosition_Y").toInt()));

    Players[0]->SetHealth(obj.value("player").toDouble());

    Players[1] = new Player(this);

    Players[1]->SetMaxSpeed(obj.value("player").toObject().value("maxSpeed").toInt());

    Players[1]->SetAcceleration(obj.value("player").toObject().value("acceleration").toInt());

    Players[1]->SetJumpAcceleration(obj.value("player").toObject().value("jumpAcceleration").toInt());

    Players[1]->SetGravity(FVector(obj.value("player").toObject().value("friction").toInt(),
        obj.value("player").toObject().value("gravity").toInt()));

    Players[1]->SetLocation(FVector(obj.value("player2").toObject().value("startPosition_X").toInt(),
        obj.value("player2").toObject().value("startPosition_Y").toInt()));

    Players[1]->SetHealth(obj.value("player").toDouble());

    connect(Players[0], SIGNAL(ChangedLocation()), this, SLOT(SendLocationPlayers()));

    connect(Players[1], SIGNAL(ChangedLocation()), this, SLOT(SendLocationPlayers()));

    connect(Players[0], SIGNAL(MakeAttack(FVector)), this, SLOT(Attack(FVector)));
    connect(Players[1], SIGNAL(MakeAttack(FVector)), this, SLOT(Attack(FVector)));
}

QJsonDocument Server::ParseToJsonPlayerLocation()
{
    FVector Player1 = Players[0]->GetLocation(), Player2 = Players[1]->GetLocation();

    QJsonObject obj1, obj2, result;
    obj1.insert("X", Player1.X);
    obj1.insert("Y", Player1.Y);
    
    result.insert("player1", obj1);

    obj2.insert("X", Player2.X);
    obj2.insert("Y", Player2.Y);

    result.insert("player2", obj2);

    return QJsonDocument(result);
}

void Server::ParseGameData()
{
    QFile file(this);
    file.setFileName(QCoreApplication::applicationDirPath() + "/settings.json");
    if (!file.open(QIODeviceBase::ReadOnly)) {
        qDebug() << "Didn't found file of game settings. Creating new file...";
        GameData = CreateBaseSettings();
        return;
    }

    GameData = ParseDataFromFile();

    attackDmg = GameData.value("player").toObject().value("damage").toDouble();
    AttackRange = GameData.value("player").toObject().value("attackRange").toDouble();
}

Server::Server(QObject* parent)
{
    ParseGameData();

    this->setParent(parent);

    SetPlayerData(GameData);
}

Server::~Server()
{
    close();

    for (auto a : Players)
        delete a;
}

void Server::Start()
{
    if (!listen(QHostAddress::Any, GameData.value("port").toInt())) {
        qDebug() << "Error: " << errorString();
        exit(1);
    }

    connect(this, SIGNAL(newConnection()), this, SLOT(AcceptNewConnection()));
}

void Server::SendLocationPlayers()
{
    auto socket1 = Players[0]->GetSocket();
    auto socket2 = Players[1]->GetSocket();

    QByteArray data = ParseToJsonPlayerLocation().toJson();
    
    if (socket1)
        socket1->write(data);
    if (socket2)
        socket2->write(data);
}

void Server::AcceptNewConnection()
{
    auto newConnection = nextPendingConnection();
    if (!newConnection)
        return;

    qDebug() << "Accepted new connection " << newConnection->peerAddress().toString();

    if (!Players[0]->GetSocket()) {
        Players[0]->SetSocket(newConnection);
        newConnection->write("P1\n");
    }
    else if (!Players[1]->GetSocket()) {
        Players[1]->SetSocket(newConnection);
        newConnection->write("P2\n");
    }
    else {
        qDebug() << "Full server, cannot accept connection " << newConnection->peerAddress() << "; Disconnect socket...";
        newConnection->close();
        delete newConnection;
    }
}
