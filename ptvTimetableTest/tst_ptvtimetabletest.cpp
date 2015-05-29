//
//  Created by Dennis Weaver on 25/03/2014.
//

#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include <ptvtimetable.h>

class PtvTimetableTest : public QObject
{
    Q_OBJECT

public:
    PtvTimetableTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCaseInitCheck();
    void testHealthCheck();

    void testPois();

    void testSearchAll();

    void testDeparturesForStop();

    void testLinesForModeTrain();
    void testLinesForModeTram();
    void testLinesForModeBus();
    void testLinesForModeVLine();
    void testLinesForModeNightrider();
    void testLinesForAllModes();


private:
    PtvTimetable * ptvtt;
    QDateTime allTestStart;
};

PtvTimetableTest::PtvTimetableTest()
{
    ptvtt = NULL;
}

void PtvTimetableTest::initTestCase()
{
    allTestStart = QDateTime::currentDateTime();
    ptvtt = new PtvTimetable("1000001", "12345678-abcd-1234-fgab-1234567890ab");
}

void PtvTimetableTest::cleanupTestCase()
{
    delete ptvtt;
    qDebug() << "All tests finished in " << allTestStart.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
}

void PtvTimetableTest::testCaseInitCheck()
{
    QVERIFY2(ptvtt != NULL, "Failed to initialise PTV timetable API");
}

void PtvTimetableTest::testHealthCheck()
{
    QDateTime start = QDateTime::currentDateTime();

    QVERIFY2( ptvtt->healthCheck(), "Healthcheck failed" );

    qDebug() << "Healthcheck took " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
}

void PtvTimetableTest::testPois()
{
    QDateTime start = QDateTime::currentDateTime();

    QVERIFY2(ptvtt->transportPOIsByMap("0,1,2,3,4,100", 90, -180, -90, 180, 0, 50 ).toMap()["locations"].toList().count() == 50, "pois for world failed");

    qDebug() << "Pois took " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
}

void PtvTimetableTest::testSearchAll()
{
    QDateTime start = QDateTime::currentDateTime();

    QVariant response = ptvtt->search("Line");
    QVERIFY2(response.toList().count() > 0, "Search all failed");

    qDebug() << "Search took " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
}

void PtvTimetableTest::testDeparturesForStop()
{
    QDateTime start = QDateTime::currentDateTime();

    QVariant response = ptvtt->departuresForMode(3, 1513, 1);
    QVERIFY2(response.toMap().count() > 0, "get departures failed");

    qDebug() << "Departures took " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
}

void PtvTimetableTest::testLinesForModeTrain()
{
    QDateTime start = QDateTime::currentDateTime();

    QVariant response = ptvtt->linesForMode(0);
    qDebug() << "Found " << response.toList().count() << " train lines in " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
    QVERIFY2(response.toList().count() > 0, "Get train lines failed");
}

void PtvTimetableTest::testLinesForModeTram()
{
    QDateTime start = QDateTime::currentDateTime();

    QVariant response = ptvtt->linesForMode(1);
    qDebug() << "Found " << response.toList().count() << " tram routes in " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
    QVERIFY2(response.toList().count() > 0, "Get tram routes failed");
}

void PtvTimetableTest::testLinesForModeBus()
{
    QDateTime start = QDateTime::currentDateTime();

    QVariant response = ptvtt->linesForMode(2);
    qDebug() << "Found " << response.toList().count() << " bus routes in " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
    QVERIFY2(response.toList().count() > 0, "Get bus routes failed");
}

void PtvTimetableTest::testLinesForModeVLine()
{
    QDateTime start = QDateTime::currentDateTime();

    QVariant response = ptvtt->linesForMode(3);
    qDebug() << "Found " << response.toList().count() << " vline lines in " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
    QVERIFY2(response.toList().count() > 0, "Get vline lines failed");
}

void PtvTimetableTest::testLinesForModeNightrider()
{
    QDateTime start = QDateTime::currentDateTime();

    QVariant response = ptvtt->linesForMode(4);
    qDebug() << "Found " << response.toList().count() << " nightrider routes in " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
    QVERIFY2(response.toList().count()> 0, "Get nightrider routes failed");
}


void PtvTimetableTest::testLinesForAllModes()
{
    QDateTime start = QDateTime::currentDateTime();

    QVariantMap response = ptvtt->linesForAllModes();
    qDebug() << "Found " << response.count() << " routes in " << start.msecsTo(QDateTime::currentDateTime())/1000.0 << " seconds";
    QVERIFY2(response.count()> 0, "Get lines for all modes failed");
}


QTEST_MAIN(PtvTimetableTest)

#include "tst_ptvtimetabletest.moc"
