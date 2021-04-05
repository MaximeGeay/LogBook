#include <QSettings>
#include <QDebug>
#include "navdata.h"
#include "ui_navdata.h"

NavData::NavData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavData)
{
    ui->setupUi(this);
    mUdpGPS=new UDPData();
    mUdpCelerite=new UDPData();
    mUdpSonde=new UDPData();
    mUdpSBE=new UDPData();
    mTimerTime=new QTimer;

    mUdpGPS->setInterval(3000);
    mUdpSonde->setInterval(3000);
    mUdpCelerite->setInterval(3000);
    mUdpSBE->setInterval(10000);

    mDateHeurePrec=QDateTime::currentDateTimeUtc();

    //initCom();
    QObject::connect(mUdpGPS,&UDPData::dataReceived,this,&NavData::readData);
    QObject::connect(mUdpGPS,&UDPData::timeout,this,&NavData::gpsTimeout);
    QObject::connect(mUdpGPS,&UDPData::errorString,this,&NavData::udpError);

    QObject::connect(mUdpSonde,&UDPData::dataReceived,this,&NavData::readData);
    QObject::connect(mUdpSonde,&UDPData::timeout,this,&NavData::sondeTimeout);
    QObject::connect(mUdpSonde,&UDPData::errorString,this,&NavData::udpError);

    QObject::connect(mUdpCelerite,&UDPData::dataReceived,this,&NavData::readData);
    QObject::connect(mUdpCelerite,&UDPData::timeout,this,&NavData::celTimeout);
    QObject::connect(mUdpCelerite,&UDPData::errorString,this,&NavData::udpError);

    QObject::connect(mUdpSBE,&UDPData::dataReceived,this,&NavData::readData);
    QObject::connect(mUdpSBE,&UDPData::timeout,this,&NavData::SBETimeout);
    QObject::connect(mUdpSBE,&UDPData::errorString,this,&NavData::udpError);

    QObject::connect(mTimerTime,&QTimer::timeout,this,&NavData::majHeure);
    mTimerTime->start(100);
}

NavData::~NavData()
{
    delete mUdpGPS;
    delete mUdpSonde;
    delete mUdpCelerite;
    delete mUdpSBE;
    delete ui;
}

void NavData::initCom()
{


    QSettings settings;
    if(mUdpGPS->UdpPort()!=0)
        mUdpGPS->close();
    if(mUdpSonde->UdpPort()!=0)
        mUdpSonde->close();
    if(mUdpCelerite->UdpPort()!=0)
        mUdpCelerite->close();
    if(mUdpSBE->UdpPort()!=0)
        mUdpSBE->close();



    mUdpGPS->initCom(settings.value("PortGPS",21006).toInt());
    mUdpSonde->initCom(settings.value("PortSonde",12044).toInt());
    mUdpCelerite->initCom(settings.value("PortCelerimetre",21035).toInt());
    mUdpSBE->initCom(settings.value("PortSBE",21021).toInt());

}

NavData::stGPSDatas NavData::getLastGPS()
{
    return mLastGPS;
}

void NavData::readData(QString sTrame)
{
     stGPSDatas uneData;
     stSBEDatas uneSBE;

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

            mLastGPS=uneData;
            emit gpsReceived(uneData);



        }

         if(sTrame.section(",",0,0)=="$INDPT" || sTrame.section(",",0,0)=="$GPDPT" || sTrame.section(",",0,0)=="$SDDPT")
         {
             mLastSonde=sTrame.section(",",1,1).toDouble();
             ui->l_Sonde->setText(QString("Sonde: %1m").arg(QString::number(mLastSonde,'f',1)));
             emit sondeReceived(mLastSonde);
         }
         if(!sTrame.contains(","))
         {
             sTrame=sTrame.remove(" \r\n");
             double dCel=sTrame.toDouble();
             if(dCel>1400&&dCel<1600)
             {
                 mLastCelerite=dCel;
                 ui->l_Celerimetre->setText(QString("Célérimètre: %1 m/s").arg(QString::number(mLastCelerite,'f',1)));
                 emit celReceived(mLastCelerite);
             }

         }

         if(sTrame.section(",",0,0)=="$SBTSG")
         {
             uneSBE.tempCuve=sTrame.section(",",10,10).toDouble();
             uneSBE.tempExt=sTrame.section(",",11,11).toDouble();
             uneSBE.conductivite=sTrame.section(",",12,12).toDouble();
             uneSBE.salinite=sTrame.section(",",13,13).toDouble();
             uneSBE.celerite=sTrame.section(",",14,14).toDouble();
             uneSBE.dispo=true;
             ui->l_salinite->setText(QString("Salinité: %1 psu").arg(uneSBE.salinite));
             ui->l_CelSBE->setText(QString("Célérité SBE: %1 m/s").arg(uneSBE.celerite));
             ui->l_Temperature->setText(QString("Température: %1 °C").arg(uneSBE.tempExt));
             mLastSBE=uneSBE;
             emit sbeReceived(mLastSBE);

         }




}

void NavData::sondeTimeout()
{
    ui->l_Sonde->setText("Sonde:");
}

void NavData::celTimeout()
{
    ui->l_Celerimetre->setText("Célérimètre:");
}

void NavData::SBETimeout()
{
    ui->l_Temperature->setText("Température:");
    ui->l_salinite->setText("Salinité:");
    ui->l_CelSBE->setText("Célérité SBE:");
    mLastSBE.dispo=false;
}

void NavData::gpsTimeout()
{
    ui->l_DateHeureGPS->setText("Données GPS non reçues");
    ui->l_Position->setText(QString("Position:"));
    ui->l_COG->setText(QString("Route fond:"));
    ui->l_SOG->setText(QString("Vitesse fond:"));
    mLastGPS.dispo=false;
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

void NavData::udpError(QString sMsg)
{
    emit errorString(sMsg);
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
