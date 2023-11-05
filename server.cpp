#include "server.h"


#define DEFAULT_MAX_SPEED 100
#define DEFAULT_ACCELERATION 10
#define DEFAULT_JUMP_ACCELERATION 100
#define DEFAULT_FLOOR_HEIGHT 420
#define DEFAULT_GRAVITY 10
#define DEFAULT_FRICTION 5
#define DEFAULT_PLAYER1_POSITION {0, 420}
#define DEFAULT_PLAYER2_POSITION {1280, 420}
#define DEFAULT_ATTACK_RANGE 300
#define DEFAULT_HEALTH 3000
#define DEFAULT_DAMAGE 10.0f
#define DEFAULT_ATTACK_SPEED 2.0f

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
    map.insert("attackSpeed", DEFAULT_ATTACK_SPEED);

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

void Server::Attack(Player* owner) {
    for (auto a : Players) {
        if ((a->GetLocation() + (owner->GetLocation() * -1)).Size() <= a->GetAttackRange() && a != owner)
            a->ReceiveDamage(a->GetDamage());
    }
}

void Server::OnDiedPlayer(Player* player) {
    isGameEnd = true;
}

void Server::OnEndGame(Player* player) {
    auto socket = player->GetSocket();

    if (!socket)
        return;

    socket->write("exit");

    delete socket;
}

void Server::SetPlayerData(QJsonObject obj)
{
    Players[0] = new Player(this);

    Players[0]->SetPlayerData(obj.value("player").toObject());

    Players[0]->SetLocation(FVector(obj.value("player1").toObject().value("startPosition_X").toInt(),
        obj.value("player1").toObject().value("startPosition_Y").toInt()));

    Players[1] = new Player(this);

    Players[1]->SetPlayerData(obj.value("player").toObject());

    Players[1]->SetLocation(FVector(obj.value("player2").toObject().value("startPosition_X").toInt(),
        obj.value("player2").toObject().value("startPosition_Y").toInt()));

    connect(Players[0], SIGNAL(ChangedLocation()), this, SLOT(SendInfoPlayerOne()));

    connect(Players[1], SIGNAL(ChangedLocation()), this, SLOT(SendInfoPlayerTwo()));

    connect(Players[0], SIGNAL(MakeAttack(Player*)), this, SLOT(Attack(Player*)));
    connect(Players[1], SIGNAL(MakeAttack(Player*)), this, SLOT(Attack(Player*)));

    connect(Players[0], SIGNAL(CastAbility(Player*, Ability*)), this, SLOT(CastAbility(Player*, Ability*)));
    connect(Players[1], SIGNAL(CastAbility(Player*, Ability*)), this, SLOT(CastAbility(Player*, Ability*)));

    connect(Players[0], SIGNAL(Died(Player*)), this, SLOT(OnDiedPlayer(Player*)));
    connect(Players[1], SIGNAL(Died(Player*)), this, SLOT(OnDiedPlayer(Player*)));
}

QJsonDocument Server::ParseToJsonPlayerInfo()
{
    float Health1 = Players[0]->GetHealth(), Health2 = Players[1]->GetHealth();
    FVector Player1 = Players[0]->GetLocation(), Player2 = Players[1]->GetLocation();

    QJsonObject obj1, obj2, result;
    QString flags = "";

    for (auto a : Players[0]->GetFlags()) {
        switch (a) {
        case EFlags::Attack:
            flags += "h";
            break;
        default:
            break;
        }
    }
    Players[0]->ClearFlags();

    obj1.insert("flags", flags);
    obj1.insert("X", (int)Player1.X);
    obj1.insert("Y", (int)Player1.Y);
    obj1.insert("health", Health1);

    result.insert("player1", obj1);

    flags = "";
    for (auto a : Players[1]->GetFlags()) {
        switch (a) {
        case EFlags::Attack:
            flags += "h";
            break;
        default:
            break;
        }
    }

    obj2.insert("flags", flags);
    obj2.insert("X", (int)Player2.X);
    obj2.insert("Y", (int)Player2.Y);
    obj2.insert("health", Health2);
    Players[1]->ClearFlags();

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
}

void Server::CastAbility(Player* owner, Ability* ability) {
    Player* enemy = nullptr;

    for (auto a : Players) {
        if (a != owner) {
            enemy = a;
            break;
        }
    }

    ability->Start(owner, enemy);
    actors.append(ability);
}

void Server::AddNewActor(Actor* actor)
{
    if (actor)
        actors.append(actor);
}

void Server::Tick()
{
    for (auto a : actors) {
        if (a)
            a->Tick(DeltaTime);
    }
}

Server::Server(QObject* parent)
{
    ParseGameData();

    this->setParent(parent);

    SetPlayerData(GameData);

    connect(&TickTimer, &QTimer::timeout, this, &Server::Tick);
    TickTimer.start(DeltaTime);
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

void Server::SendInfoPlayerTwo() {
    auto socket = Players[1]->GetSocket();

    if (isGameEnd) {
        OnEndGame(Players[1]);
        return;
    }

    QByteArray data = ParseToJsonPlayerInfo().toJson();

    if (socket && socket->isWritable())
        socket->write(data);
}

void Server::SendInfoPlayerOne()
{
    auto socket = Players[0]->GetSocket();

    if (isGameEnd) {
        OnEndGame(Players[0]);
        return;
    }

    QByteArray data = ParseToJsonPlayerInfo().toJson();

    if (socket && socket->isWritable())
        socket->write(data);
}

void Server::AcceptNewConnection()
{
    auto newConnection = nextPendingConnection();
    if (!newConnection)
        return;

    qDebug() << "Accepted new connection " << newConnection->peerAddress().toString();

    if (!Players[0]->GetSocket())
        Players[0]->SetSocket(newConnection);
    else if (!Players[1]->GetSocket())
        Players[1]->SetSocket(newConnection);
    else {
        qDebug() << "Full server, cannot accept connection " << newConnection->peerAddress() << "; Disconnect socket...";
        newConnection->close();
        delete newConnection;
    }
}
