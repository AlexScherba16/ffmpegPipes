#include "fileparser.h"
#include <QDebug>
#include <QDir>
#include <QThread>
#include <QLocalSocket>

#include "common_types.h"

qint64 FileParser::_readBasketHeader(QFile &basket, headFileBusket &header){
    qint64 header_size = sizeof(header);
    memset((char*)&header, 0, header_size);
    if(header_size == basket.read((char*)&header, header_size))
        return header_size;
    return 0;
}

qint64 FileParser::_readFrameHeader(QFile &file, headFrameBusket &frame){
    qint64 frame_size = sizeof(frame);
    memset((char*)&frame, 0, frame_size);
    if(frame_size == file.read((char*)&frame, frame_size))
        return frame_size;
    return 0;
}

bool FileParser::_needNalPrefix(const QByteArray &data)
{
    auto symbol = QString("0x%1").arg((int)data[0], 0, 16);
    if(symbol == "0x67")    //SPS
        return true;
    if(symbol == "0x68")    //PPS
        return true;
    if(symbol == "0x6")     //SD
        return true;
    if(symbol == "0x65")    //Picture
        return true;
    if(symbol == "0x61")    //Variation
        return true;
    return false;
}

void FileParser::_unpack_basket(const QString &path, QLocalSocket* rx_pipe)
{
    qDebug() << "[FileParser][unpack_basket] " << path;

    QFile raw(path);
    if(!raw.open(QIODevice::ReadOnly)){
        qDebug() << "[FileParser]                   ERROR_OPEN " << path;
        return;
    }

    headFileBusket head;
    if(_readBasketHeader(raw, head) != sizeof(headFileBusket)){
        qDebug() << "[FileParser]                   ERROR_BASK_HEADER " << path;
        return;
    }

    headFrameBusket frame;
    QByteArray dataChunk;
    do{
        if(_readFrameHeader(raw, frame) != sizeof(headFrameBusket)){
            qDebug() << "[FileParser]               ERROR_FRAME_HEADER " << path;
            raw.close();
            return;
        }
        if(((EType)frame.type == VIDEO && frame.channel == 1)){
            auto data = raw.read(frame.frame_size);

            if(_needNalPrefix(data)){
                quint8 nalChar = 0;
                data.prepend(nalChar);
                data.prepend(nalChar);
                data.prepend(nalChar);

                nalChar = 1;
                data.prepend(nalChar);
            }

            _tx_size += data.size();
//            dataChunk.append(data);
//            if(dataChunk.size() > 250000){
                rx_pipe->write(data); //dataChunk);
                rx_pipe->flush();
//                dataChunk.clear();
//            }
        }
        else
            raw.seek(raw.pos() + frame.frame_size);
    }while(raw.pos() != head.shift_period);

//    if(dataChunk.size()){
//        rx_pipe->write(dataChunk);
//        rx_pipe->flush();
//    }
}

FileParser::FileParser(QObject *parent) : QObject(parent)
{
    qDebug() << "[FileParser]  Constructor"; 
}

FileParser::~FileParser()
{
    qDebug() << "[FileParser]  Destructor";
}

void FileParser::parseBaskFiles(const QString& storagePath, QLocalSocket* rx_pipe){
    QDir directory(storagePath);
    auto baskets = directory.entryInfoList(QStringList() << "bask_*", QDir::Files);

//    connect(clientConnection, &QLocalSocket::disconnected,
//            clientConnection, &QLocalSocket::deleteLater);

    _tx_size = 0;
    for(auto it = baskets.constBegin(); it != baskets.constEnd(); ++it){
        _unpack_basket(it->filePath(), rx_pipe);
        qDebug() << "______________________";
        break;
    }
    emit parser_finished(_tx_size);
}
