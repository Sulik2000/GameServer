#include "server.h"

#define DEFAULT_MAX_SPEED 100
#define DEFAULT_ACCELERATION 10
#define DEFAULT_JUMP_ACCELERATION 100
#define DEFAULT_FLOOR_HEIGHT 420
#define DEFAULT_GRAVITY 10
#define DEFAULT_FRICTION 5
#define DEFAULT_PLAYER1_POSITION {0, 420}
#define DEFAULT_PLAYER2_POSITION {1280, 420}

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

Server::Server(QObject* parent)
{
    ParseGameData();

    this->setParent(parent);

    Players[0] = new Player(this);

    Players[0]->SetMaxSpeed(GameData.value("player").toObject().value("maxSpeed").toInt());

    Players[0]->SetAcceleration(GameData.value("player").toObject().value("acceleration").toInt());

    Players[0]->SetJumpAcceleration(GameData.value("player").toObject().value("jumpAcceleration").toInt());

    Players[0]->SetGravity(FVector(GameData.value("player").toObject().value("friction").toInt(),
        GameData.value("player").toObject().value("gravity").toInt()));

    Players[0]->SetLocation(FVector(GameData.value("player1").toObject().value("startPosition_X").toInt(),
        GameData.value("player1").toObject().value("startPosition_Y").toInt()));

    connect(Players[0], SIGNAL(ChangedLocation()), this, SLOT(SendLocationPlayers()));

    Players[1] = new Player(this);

    Players[1]->SetMaxSpeed(GameData.value("player").toObject().value("maxSpeed").toInt());

    Players[1]->SetAcceleration(GameData.value("player").toObject().value("acceleration").toInt());

    Players[1]->SetJumpAcceleration(GameData.value("player").toObject().value("jumpAcceleration").toInt());

    Players[1]->SetGravity(FVector(GameData.value("player").toObject().value("friction").toInt(),
        GameData.value("player").toObject().value("gravity").toInt()));

    Players[1]->SetLocation(FVector(GameData.value("player2").toObject().value("startPosition_X").toInt(),
        GameData.value("player2").toObject().value("startPosition_Y").toInt()));

    connect(Players[1], SIGNAL(ChangedLocation()), this, SLOT(SendLocationPlayers()));
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

    QString str = "<" + QString::number((int)Players[0]->GetLocation().X) + ", "
        + QString::number((int)Players[0]->GetLocation().Y) + ">";
    if (socket1)
        socket1->write(str.toUtf8());
    if (socket2)
        socket2->write(str.toUtf8());
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
