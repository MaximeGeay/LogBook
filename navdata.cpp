#include <QSettings>
#include <QDebug>
#include "navdata.h"
#include "ui_navdata.h"

NavData::NavData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavData)
{
    ui->setupUi(this);
    mPortGPS=0;
    mGPSSocket=new QUdpSocket();
    mTimer=new QTimer;
    mTimerTime=new QTimer;

    mTimer->setInterval(3000);
    mTimer->setSingleShot(true);
    mDateHeurePrec=QDateTime::currentDateTimeUtc();

    initCom();
    QObject::connect(mGPSSocket,&QUdpSocket::readyRead,this,&NavData::readData);
    QObject::connect(mTimer,&QTimer::timeout,this,&NavData::dataTimeout);
    QObject::connect(mTimerTime,&QTimer::timeout,this,&NavData::majHeure);
    mTimerTime->start(100);
}

NavData::~NavData()
{
    mGPSSocket->close();
    delete ui;
}

void NavData::initCom()
{
    bool bRes=false;
    QSettings settings;
    if(mPortGPS!=0)
    {
        //close connection
        mGPSSocket->close();
    }

    mPortGPS=settings.value("PortGPS",20005).toInt();

    if(mGPSSocket->bind(QHostAddress::Any,mPortGPS))
    {
         emit errorString(QString("Connecté au port UDP %1").arg(mPortGPS));
        bRes=true;
    }
    else
    {
        emit errorString(mGPSSocket->errorString());
        bRes=false;
    }
}

NavData::Datas NavData::getLastData()
{
    return mLastData;
}

void NavData::readData()
{
     Datas uneData;
    while (mGPSSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(mGPSSocket->pendingDatagramSize());
        mGPSSocket-> readDatagram(datagram.data(),datagram.size()); // datagram.data est un char
        QString sTrame=datagram.data();  // convertion qbytearray en string
       // emit dataReceived(mParam.Name,sTrame); // transmission du signal avec la trame recu


        /*
$GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
225446 = Heure du Fix 22:54:46 UTC
A = Alerte du logiciel de navigation ( A = OK, V = warning (alerte)
4916.45,N = Latitude 49 deg. 16.45 min North
12311.12,W = Longitude 123 deg. 11.12 min West
000.5 = vitesse sol, Noeuds
054.7 = cap (vrai)
191194 = Date du fix 19 Novembre 1994
020.3,E = Déclinaison Magnétique 20.3 deg Est
*68 = checksum obligatoire
*/
        if(sTrame.section(",",0,0)=="$INRMC" || sTrame.section(",",0,0)=="$GPRMC")
        {
            QString sDateHeure=sTrame.section(",",1,1).section(".",0,0)+" "+sTrame.section(",",9,9);
            QString sLat=sTrame.section(",",3,3);
            QString sLatDeg= sLat.left(2);
            QString sLatMin=sLat.remove(0,2);
            QString sLatHemi=sTrame.section(",",4,4);
            QString sLatitude=QString("%1,%2,%3").arg(sLatHemi).arg(sLatDeg).arg(sLatMin);
            QString sLong=sTrame.section(",",5,5);
            QString sLongDeg= sLong.left(3);
            QString sLongMin=sLong.remove(0,3);
            QString sLongHemi=sTrame.section(",",6,6);
            QString sLongitude=QString("%1,%2,%3").arg(sLongHemi).arg(sLongDeg).arg(sLongMin);
            QString sSOG=sTrame.section(",",7,7);
            QString sCOG=sTrame.section(",",8,8);
            uneData.dateheure=QDateTime::fromString(sDateHeure,"hhmmss ddMMyy");
            uneData.dateheure=uneData.dateheure.addYears(100);
            uneData.dateheure.setTimeSpec(Qt::UTC);
            uneData.position.setLatitude(latMinToDec(sLatitude));
            uneData.position.setLongitude(longMinToDec(sLongitude));
            uneData.SOG=sSOG.toDouble();
            uneData.COG=sCOG.toDouble();
            uneData.dispo=true;

            ui->l_DateHeureGPS->setText(QString("Heure GPS : %1").arg(uneData.dateheure.toString("dd/MM/yyyy hh:mm:ss")));
            ui->l_Position->setText(QString("Position: %1").arg(uneData.position.toString()));
            ui->l_COG->setText(QString("Route fond: %1°").arg(QString::number(uneData.COG,'f',0)));
            ui->l_SOG->setText(QString("Vitesse fond: %1 nds").arg(QString::number(uneData.SOG,'f',1)));

            mLastData=uneData;
            emit dataReceived(uneData);
            mTimer->start();


        }

    }
}

void NavData::dataTimeout()
{
    ui->l_DateHeureGPS->setText("Données non reçues");
    ui->l_Position->setText(QString("Position:"));
    ui->l_COG->setText(QString("Route fond:"));
    ui->l_SOG->setText(QString("Vitesse fond:"));
    mLastData.dispo=false;
}

void NavData::majHeure()
{
    QDateTime currentTime=QDateTime::currentDateTimeUtc();
    if(currentTime.time().second()!=mDateHeurePrec.time().second())
    {
        mDateHeurePrec=currentTime;
        ui->l_dtPC->setText(QString("Heure PC: %1").arg(currentTime.toString("dd/MM/yyyy hh:mm:ss")));
    }
}

double NavData::latMinToDec(QString sLatitude)
{
    //format : H,DD,MM.ddddd,
        //         S,12,30.54443
        int nDeg=sLatitude.section(",",1,1).toInt();
        double dMin=sLatitude.section(",",2,2).toDouble();
        QString cHemi=sLatitude.section(",",0,0);

        double dLatDec=nDeg+(dMin/60);
        if(cHemi=="S")
            dLatDec=-dLatDec;

        return dLatDec;

}


double NavData::longMinToDec(QString sLongitude)
{
    //format :H,DDD,MM.ddddd
    //        E,044,55.20103
    int nDeg=sLongitude.section(",",1,1).toInt();
    double dMin=sLongitude.section(",",2,2).toDouble();
    QString cHemi=sLongitude.section(",",0,0);

    double dLongDec=nDeg+(dMin/60);
    if(cHemi=="W")
        dLongDec=-dLongDec;

    return dLongDec;
}
