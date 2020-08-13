#include "backend.h"
#include <QDebug>

void Backend::_onFFmpegConnected(){
    QLocalSocket* pipe = _communication_server.nextPendingConnection();
    if(pipe == nullptr){
        qDebug() << "[Backend] no pending connections";
        return;
    }
    emit launchParsing("/media/alex/01D39ACE5091F6C0", pipe);
    emit runFFmpegPlayer();
}

void Backend::_onParsingFinished(qint64 tx_size)
{
    qDebug() << "[Backend] parsing finished " << tx_size << _ffmpeg._rx_size;
}

Backend::Backend(QObject *parent) : QObject(parent)
{
    qDebug() << "[Backend]  Constructor";

    if(!_communication_server.listen("bask_server")){
        qDebug() << "[Backend] server listen error  " << _communication_server.errorString();
        QLocalServer::removeServer("bask_server");
        throw 228;
    }

    _parser.moveToThread(&_parserThread);
    _parserThread.start();
    while (!_parserThread.isRunning()){};
    connect(this, &Backend::launchParsing, &_parser, &FileParser::parseBaskFiles);
    connect(&_parser, &FileParser::parser_finished, this, &Backend::_onParsingFinished);


    _ffmpeg.moveToThread(&_ffmpegThread);
    _ffmpegThread.start();
    while (!_ffmpegThread.isRunning()){};

    connect(this, &Backend::setConnection, &_ffmpeg, &FfmpegManager::set_connection);
    connect(this, &Backend::runFFmpegPlayer, &_ffmpeg, &FfmpegManager::play);
    connect(&_ffmpeg, &FfmpegManager::connection_established, this, &Backend::_onFFmpegConnected);
}

Backend::~Backend()
{
    qDebug() << "[Backend]  Destructor";

    _parserThread.quit();
    _parserThread.wait();

    _ffmpegThread.quit();
    _ffmpegThread.wait();

    _communication_server.close();
    QLocalServer::removeServer("bask_server");
}

void Backend::launchPlayer(){
    emit setConnection(_communication_server.serverName());
}


//QLocalSocket *clientConnection = server->nextPendingConnection();
//connect(clientConnection, &QLocalSocket::disconnected,
//        clientConnection, &QLocalSocket::deleteLater);

//clientConnection->write(block);
//clientConnection->flush();
//clientConnection->disconnectFromServer();
