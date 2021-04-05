#ifndef NAVDATA_H
#define NAVDATA_H

#include <QWidget>
#include <QGeoCoordinate>
#include <QDateTime>
#include <QTimer>
#include "udpdata.h"

namespace Ui {
class NavData;
}

class NavData : public QWidget
{
    Q_OBJECT

public:

    struct stGPSDatas{
        QGeoCoordinate position;
        double SOG=0,COG=0;
        QDateTime dateheure;
        bool dispo=false;
    };
    struct stSBEDatas{
        double tempCuve=0;
        double tempExt=0;
        double salinite=0;
        double conductivite=0;
        double celerite=0;
        bool dispo=false;

    };
    struct stSensorsDatas{
        stGPSDatas GPS;
        stSBEDatas SBE;
        double depth=0;
        double celerimeter=0;
    };

    explicit NavData(QWidget *parent = nullptr);
    ~NavData();



signals:
    void gpsReceived(stGPSDatas);
    void sondeReceived(double);
    void celReceived(double);
    void sbeReceived(stSBEDatas);
    void errorString(QString);

public slots:
    void initCom();
    stGPSDatas getLastGPS();

private slots:
     void readData(QString sTrame);
     void gpsTimeout();
     void sondeTimeout();
     void celTimeout();
     void SBETimeout();
     void majHeure();
     void udpError(QString sMsg);

private:
    Ui::NavData *ui;

    UDPData *mUdpGPS;
    UDPData *mUdpSonde;
    UDPData *mUdpCelerite;
    UDPData *mUdpSBE;
    double latMinToDec(QString sLatitude);
    double longMinToDec(QString sLongitude);

    QTimer *mTimerTime;
    stGPSDatas mLastGPS;
    stSBEDatas mLastSBE;
    double mLastCelerite=0;
    double mLastSonde=0;
    QDateTime mDateHeurePrec;
};

#endif // NAVDATA_H
