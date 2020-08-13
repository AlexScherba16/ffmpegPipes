#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <QString>
#include <QVector>
#include <QMap>


typedef struct
{
    qint32 start_time;
    qint32 last_time;
    quint32 len;
    quint32 shift_period;
    quint32 res;
}headFileBusket;

typedef struct
{
    qint32 channel;
    quint32 frame_size;
    qint64 s_start;
    qint64 us_start;
    qint32 type;
    quint32 reserved;
}headFrameBusket;

typedef struct
{
    qint32 channel;
    qint32 value;
}headPeriods;

typedef struct
{
    quint32 first_frame_time;
    quint32 last_frame_time;
}period;

typedef enum
{
    VIDEO = 0,
    META = 1,
    AUDIO = 16,
    ADDITIONAL = 119
}EType;

typedef enum
{
    COORDINATES = 1,
    SPEED,
    FACES,
}EMetaType;


typedef struct
{
    quint64 seek;
    quint32 size;
}filePosition;

using Date = QString;
using Camera = QString;
using VideoCameras = QVector<QString>;
using FilePositions = QVector<filePosition>;
using MediaData = QMap<EType, FilePositions>;
using Chennals = QMap<QString, MediaData>;
using ArchiveRecord = QPair<QString, QString>;

// TODO: move to class increase usability
typedef struct
{
    QString path;
    QString name;
    QString full;
    Chennals chennals;
}Record;

typedef struct
{
   QString format;
   QString exportPath;
   QString chunk;
}converterMetaInfo;

typedef struct
{
   QString srcMedia;
   ArchiveRecord record;
}MediaUnitInfo;


#endif // COMMON_TYPES_H
