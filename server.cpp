#include "server.h"

void Server::GetDataFromSocket(QTcpSocket* socket)
{
    while (true) {

        socket->waitForReadyRead();
        QByteArray ByteArray = socket->read(socket->bytesAvailable());
        if(ByteArray.isNull())
            continue;
        QString string = QString::fromUtf8(ByteArray);
        qDebug() << string;
        QJsonParseError error;
        QJsonDocument JsonDoc = QJsonDocument::fromJson(ByteArray, &error);
        if (error.error != error.NoError) {
            qDebug() << "Failed get information from " << socket->localAddress().toString() << " socket. Disconnect socket...\n" << error.errorString();
            socket->close();
            delete socket;
            break;
        }
        QJsonObject JsonArray = JsonDoc.object();

        ParseInput(JsonArray);
    }
}

Server::Server(QObject* parent)
{
    this->setParent(parent);

    connect(this, SIGNAL(getConnection(QTcpSocket*)), this, SLOT(GetDataFromSocket(QTcpSocket*)));
    connect(this, SIGNAL(newConnection()), this, SLOT(AcceptNewConnection()));

    Players[0] = new Player(this);
    Players[1] = new Player(this);
    connections[0] = nullptr;
    connections[1] = nullptr;
}

Server::~Server()
{
    close();

    for (auto a : Players)
        delete a;

    for (auto a : Threads)
        delete a;
}

void Server::Start()
{
    if (!listen(QHostAddress::Any, PORT)) {
        qDebug() << "Error: " << errorString();
        exit(1);
    }

    
}

void Server::AcceptNewConnection()
{
    auto newConnection = nextPendingConnection();
    if(!newConnection)
        return;

    qDebug() << "Accepted new connection " << newConnection->peerAddress().toString();

    // Test
    if (newConnection->isWritable())
        newConnection->write("Hello, I'm C++");


    if (!connections[0]) {
        connections[0] = newConnection;
    }

    else if (!connections[1]) {
        connections[1] = newConnection;
    }

    else {
        qDebug() << "Max connections on server";
        newConnection->close();
        return;
    }

    emit getConnection(newConnection);
}

void Server::ParseInput(QJsonObject object)
{
    QVariantMap InputMap = object.toVariantMap();
    if (InputMap["r1"].toBool())
        Players[0]->AddCommand(Commands::MoveRight);
    if (InputMap["l1"].toBool())
        Players[0]->AddCommand(Commands::MoveLeft);
    if (InputMap["d1"].toBool())
        Players[0]->SetLookSide(LookSide::Right);

    if (InputMap["r2"].toBool())
        Players[1]->AddCommand(Commands::MoveRight);
    if (InputMap["l2"].toBool())
        Players[1]->AddCommand(Commands::MoveLeft);
    if (InputMap["d2"].toBool())
        Players[1]->SetLookSide(LookSide::Right);
}
