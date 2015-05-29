#include "ptvtimetable.h"

#include <QNetworkAccessManager>
#include <QUrl>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QCryptographicHash>
#include <QDateTime>
#include <qjson/parser.h>
#include <QtConcurrentMap>

#define THREAD_MULTIPLIER 20

PtvTimetable::PtvTimetable()
{
    throw;
}

PtvTimetable::PtvTimetable(QString devid, QString key)
{
    mDevId = devid;
    mKey = key;
    mBaseURL= "http://timetableapi.ptv.vic.gov.au";
}

PtvTimetable::PtvTimetable(QString devid, QString key, QString baseUrl)
{
    mDevId = devid;
    mKey = key;
    mBaseURL = baseUrl;
}

int PtvTimetable::ttToMode(QString modeString)
{
    int mode;

    if( modeString == "train" )
        mode = 0;
    else if( modeString == "tram" )
        mode =1;
    else if( modeString == "bus" )
        mode =2;
    else if( modeString == "vline" )
        mode = 3;
    else if( modeString == "nightrider")
        mode = 4;
    else
        throw;
    return mode;
}


bool PtvTimetable::healthCheck()
{
    QString request = "/v2/healthcheck?timestamp=" + QDateTime::currentDateTime().toUTC().toString( Qt::ISODate);
    QVariant response = doAPIcall( request );

    return  response.toMap()["clientClockOK"].toBool() && response.toMap()["databaseOK"].toBool()
            && response.toMap()["memcacheOK"].toBool() && response.toMap()["securityTokenOK"].toBool();
}

QVariant PtvTimetable::transportPOIsByMap(QString poiTypes, float lat1, float long1, float lat2, float long2, int gridDepth, int limit )
{

    QString request;

    QTextStream(&request) << "/v2/poi/" << poiTypes
                             << "/lat1/" << lat1 << "/long1/" << long1
                             << "/lat2/" << lat2 << "/long2/" << long2
                             << "/griddepth/" << gridDepth << "/limit/" << limit;

    QVariant response = doAPIcall( request );

    return response;
}


QVariant PtvTimetable::search( QString searchText )
{
    return doAPIcall( QString( "/v2/search/" ) + searchText );
}


QVariant PtvTimetable::departuresForMode( int mode, int stop, int limit )
{
    QString request;
    QTextStream(&request) << "/v2/mode/" << mode << "/stop/" << stop << "/departures/by-destination/limit/" << limit;

    return doAPIcall( request );
}

QVariant PtvTimetable::departuresForMode( int mode, int stop, int limit, QDateTime from)
{
    QString request;
    QTextStream(&request) << "/v2/mode/" << mode << "/stop/" << stop << "/departures/by-destination/limit/" << limit << "?for_utc=" << from.toUTC().toString( Qt::ISODate);

    return doAPIcall( request );
}

struct mappedLinesForMode
{
    mappedLinesForMode(PtvTimetable * ttapi)
        : m_ttapi(ttapi) { }

    typedef  QList< QVariant > result_type;


    result_type operator()(int mode )
    {
        return m_ttapi->linesForMode(mode).toList();
    }

    PtvTimetable * m_ttapi;
};


void reducedLines( QVariantMap &finalSet, QList< QVariant > thisSet )
{
    foreach( QVariant line, thisSet )
    {
        QString line_id = line.toMap()["line_id"].toString();

        if( ! finalSet.contains( line_id ))
            finalSet[line_id] = line;
    }
}

QVariant PtvTimetable::linesForMode(int mode )
{
        QString request;
        QTextStream(&request) << "/v2/lines/mode/" << mode;

        return doAPIcall( request );
}

QVariantMap PtvTimetable::linesForAllModes( )
{
    //modes are 0,1,2,3,4

    QList<int> modes;
    modes.append(0);
    modes.append(1);
    modes.append(2);
    modes.append(3);
    modes.append(4);

    QVariantMap lines = QtConcurrent::blockingMappedReduced( modes, mappedLinesForMode(this), reducedLines );


    return lines;
}



QVariant PtvTimetable::doAPIcall( const QString apiCall)
{
    bool ok = false;
    int retry = 500;
    QUrl url = QUrl( mBaseURL + signApiCall( apiCall ) );
    QVariant data;

    // qDebug() << url;

    while( retry > 0)
    {
        QEventLoop loop;
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        QNetworkReply *reply;

        connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
        reply =  manager->get( QNetworkRequest( url) );
        loop.exec();

        QJson::Parser parser;

        if( reply->error() == QNetworkReply::NoError )
        {
            QByteArray response = reply->readAll();
            data = parser.parse(response, &ok );
        }


        if( !ok )
        {
            qDebug() << "Network error " << reply->error();
            --retry;
        }
        else
        {
            retry = 0;
        }

        reply->deleteLater();
        manager->deleteLater();
    }

    return data;
}

QString PtvTimetable::signApiCall( const QString apiCall)
{
    QString signedApiCall;
    QString signature;

    signedApiCall = apiCall;
    if( apiCall.contains( '?') )
        signedApiCall += "&devid=" + mDevId;
    else
         signedApiCall += "?devid=" + mDevId;

    signature = hmacSha1( mKey.toAscii(), signedApiCall.toAscii() );

    signedApiCall += "&signature=" + signature;

    return signedApiCall;
}



// shamelessly stolen from http://qt-project.org/wiki/HMAC-SHA1
QString PtvTimetable::hmacSha1(QByteArray key, QByteArray baseString)
{
    int blockSize = 64; // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) { // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }

    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "\"
    // ascii characters 0x36 ("6") and 0x5c ("\") are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)

    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR operation between every byte in key and outerpadding, of key length
    }

    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
    return hashed.toHex().toUpper();
}
