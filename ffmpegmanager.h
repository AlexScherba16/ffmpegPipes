#ifndef FFMPEGMANAGER_H
#define FFMPEGMANAGER_H

#include <QObject>
#include <QLocalSocket>
#include <QtNetwork>
#include <QProcess>

class FfmpegManager : public QObject
{
    Q_OBJECT
private:
    QLocalSocket* _rx_socket;
    QProcess *mPlayProcess;



private slots:
    void _read_incomming_data();
    void _read_error(QLocalSocket::LocalSocketError socketError);
    void _onErrorOccurred(QProcess::ProcessError error);


    void _onPlayerStart();
    void _onPlayerFinished(int result);


    void _readProcessError();
    void _readProcessOutput();


public:
    explicit FfmpegManager(QObject *parent = nullptr);
    ~FfmpegManager();
    qint64 _rx_size;

public slots:
    void set_connection(QString server);
    void play();

signals:
    void connection_established();

};

#endif // FFMPEGMANAGER_H
