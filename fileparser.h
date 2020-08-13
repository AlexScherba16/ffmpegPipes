#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QObject>
#include <QFile>
#include "common_types.h"

class QLocalSocket;

class FileParser : public QObject
{
    Q_OBJECT

private:
    bool _needNalPrefix(const QByteArray &data);
    qint64 _readBasketHeader(QFile &basket, headFileBusket &header);
    qint64 _readFrameHeader(QFile &file, headFrameBusket &frame);
    void _unpack_basket(const QString& path, QLocalSocket* rx_pipe);

    qint64 _tx_size;


public:
    explicit FileParser(QObject *parent = nullptr);
    ~FileParser();


public slots:
    void parseBaskFiles(const QString& storagePath, QLocalSocket* rx_pipe);

signals:
    void parser_finished(qint64 transmited_size);


};

#endif // FILEPARSER_H
