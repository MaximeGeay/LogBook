#ifndef NAVDATA_H
#define NAVDATA_H

#include <QWidget>
#include <QUdpSocket>
#include <QGeoCoordinate>
#include <QDateTime>
#include <QTimer>

namespace Ui {
class NavData;
}

class NavData : public QWidget
{
    Q_OBJECT

public:

    struct Datas{
        QGeoCoordinate position;
        double SOG=0,COG=0;
        QDateTime dateheure;
        bool dispo=false;
    };

    explicit NavData(QWidget *parent = nullptr);
    ~NavData();



signals:
    void dataReceived(Datas);
    void errorString(QString);

public slots:
    void initCom();
    Datas getLastData();

private slots:
     void readData();
     void dataTimeout();
     void majHeure();

private:
    Ui::NavData *ui;
    int mPortGPS;
    QUdpSocket* mGPSSocket;
    double latMinToDec(QString sLatitude);
    double longMinToDec(QString sLongitude);
    QTimer * mTimer;
    QTimer *mTimerTime;
    Datas mLastData;
    QDateTime mDateHeurePrec;
};

#endif // NAVDATA_H
