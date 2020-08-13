#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QtNetwork>
#include <QThread>

#include "fileparser.h"
#include "ffmpegmanager.h"


class Backend : public QObject
{
    Q_OBJECT
private:
    FileParser _parser;
    FfmpegManager _ffmpeg;
    QLocalServer _communication_server;

    QThread _parserThread;
    QThread _ffmpegThread;


private slots:
    void _onFFmpegConnected();
    void _onParsingFinished(qint64 tx_size);


public:
    explicit Backend(QObject *parent = nullptr);
    ~Backend();

    void launchPlayer();

signals:
    void setConnection(const QString& serverName);
    void launchParsing(const QString& filePath, QLocalSocket* tx_pipe);

    void runFFmpegPlayer();
    void done();

};

#endif // BACKEND_H
