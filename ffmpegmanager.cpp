#include "ffmpegmanager.h"
#include <QDebug>
#include <QtNetwork>

void FfmpegManager::_onPlayerStart()
{
    qDebug() << "[FfmpegManager]   PLAYER_PROCESS_STARTED";
}

void FfmpegManager::_onPlayerFinished(int result)
{
    qDebug() << "[FfmpegManager]   PLAYER_PROCESS_FINISHED     " << result;
}

void FfmpegManager::_readProcessError()
{
    qDebug() << "[FfmpegManager]   PROCESSSS____ERROR     ";
}

void FfmpegManager::_readProcessOutput()
{
    qDebug() << mPlayProcess->readAllStandardOutput();
}


void FfmpegManager::_read_incomming_data(){

//    _rx_socket->bytesAvailable()

    auto data = _rx_socket->readAll();
    _rx_size += data.size();
//    qDebug() << "[FFMPEG]   read data " << _rx_size;//<< data.size();


    if(mPlayProcess->isWritable()){
        mPlayProcess->write(data);

    }
    else{
        qDebug() << "\t[FFMPEG] PPPPPPPPP" << mPlayProcess->state();
        mPlayProcess->closeWriteChannel();
    }
//    if(!mPlayProcess->waitForBytesWritten(30000)){
//    }
}

void FfmpegManager::_read_error(QLocalSocket::LocalSocketError socketError){
    switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
        qDebug() << "QLocalSocket::ServerNotFoundError\n" << tr("The host was not found. Please make sure "
                                    "that the server is running and that the "
                                    "server name is correct.");
        break;
    case QLocalSocket::ConnectionRefusedError:
        qDebug() << "QLocalSocket::ConnectionRefusedError" << tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the server name "
                                    "is correct.");
        break;
    case QLocalSocket::PeerClosedError:
        qDebug() << "QLocalSocket::PeerClosedError";
        break;
    default:
        qDebug() << "ERROR " << _rx_socket->errorString();
    }
}

void FfmpegManager::_onErrorOccurred(QProcess::ProcessError error)
{
     qDebug() << "[FfmpegManager]   ERRRRRRRRRRRRRRRR     " << error;
}

FfmpegManager::FfmpegManager(QObject *parent) : QObject(parent){
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    _rx_socket = nullptr;

    qDebug() << "[FfmpegManager]  Constructor";
}

FfmpegManager::~FfmpegManager(){
    qDebug() << "[FfmpegManager]  Destructor";
    if(_rx_socket){
        _rx_socket->abort();
        delete _rx_socket;
    }

    if(mPlayProcess != NULL){
        mPlayProcess->kill();
        mPlayProcess->waitForFinished();
    }
}

void FfmpegManager::set_connection(QString server)
{
    if(_rx_socket){
        _rx_socket->abort();
        delete _rx_socket;
    }

    _rx_socket = new QLocalSocket(this);
    _rx_socket->setReadBufferSize(50000000);
    _rx_socket->connectToServer(server, QIODevice::ReadWrite);

    if(_rx_socket->waitForConnected()){
        qDebug() << "[FfmpegManager]  connected to ... " << server;
        QThread::msleep(5);
        emit connection_established();
        return;
    }
    qDebug() << "[FfmpegManager]        connection_error";
}

void FfmpegManager::play()
{
    mPlayProcess = new QProcess(this);
    connect(mPlayProcess, SIGNAL(started()), this, SLOT(_onPlayerStart()));
    connect(mPlayProcess, SIGNAL(finished(int)), this, SLOT(_onPlayerFinished(int)));
    connect(mPlayProcess, &QProcess::readyReadStandardError, this, &FfmpegManager::_readProcessError);
    connect(mPlayProcess, &QProcess::readyReadStandardOutput, this, &FfmpegManager::_readProcessOutput);


//    connect(mPlayProcess, SIGNAL(errorOccurred(QProcess::ProcessError error)),
//            this, SLOT(_onErrorOccurred(QProcess::ProcessError error)));



//    Stream #0:0: Video: h264 (Baseline), yuvj420p(pc, bt709, progressive), 1920x1080, 25 fps, 25 tbr, 1200k tbn, 50 tbc


    QStringList arguments;
    arguments << "-f" << "h264" <<
                 "-pixel_format" << "yuvj420p" <<
                 "-video_size" << "1920x1080" <<
                 "-b" << "1200k" <<
                 "-i" << "pipe:0";
//    arguments << "-i" << "pipe:0";

    mPlayProcess->setCurrentWriteChannel(0);
    mPlayProcess->setCurrentReadChannel(0);
    mPlayProcess->setProcessChannelMode(QProcess::MergedChannels);
    mPlayProcess->start("ffplay", arguments, QIODevice::ReadWrite);
    QThread::msleep(5);

    connect(_rx_socket, &QLocalSocket::errorOccurred, this, &FfmpegManager::_read_error);
    connect(_rx_socket, &QLocalSocket::readyRead, this, &FfmpegManager::_read_incomming_data);
}


////    if(media.path.isAudioOk)
////        arguments << "-f" << "alaw" << "-i" << media.path.audioPath << "-filter:a"  << "asetrate=r=8K";
