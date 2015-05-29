//
//  Created by Dennis Weaver on 25/03/2014.
//

#ifndef PTVTIMETABLE_H
#define PTVTIMETABLE_H

#include <QString>
#include <QVariant>

class PtvTimetable :  public QObject
{

public:
    PtvTimetable();
    PtvTimetable(QString, QString);
    PtvTimetable(QString, QString, QString);

    static int ttToMode(QString modeString);

    bool healthCheck();
    QVariant transportPOIsByMap(QString poiTypes, float lat1, float long1, float lat2, float long2, int gridDepth, int limit );

    QVariant search( QString searchText );
    QVariant departuresForMode(int mode, int stop, int limit);
    QVariant departuresForMode(int mode, int stop, int limit , QDateTime from);

    QVariant linesForMode(int mode );
    QVariantMap linesForAllModes( );


protected:
    QVariant doAPIcall(const QString);
    QString signApiCall(const QString );
    QString hmacSha1(QByteArray key, QByteArray baseString);



private:
    QString mDevId;
    QString mKey;
    QString mBaseURL;
};

#endif // PTVTIMETABLE_H
