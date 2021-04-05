#include <QMessageBox>
#include <QDebug>
#include <QSettings>

#include "eventmanager.h"
#include "ui_eventmanager.h"


EventManager::EventManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventManager)
{
    ui->setupUi(this);

    mEventDetails=new EventDetails;
    mLayoutEventDetails=new QVBoxLayout();
    mLayoutEventDetails->setAlignment(Qt::AlignTop);
    mLayoutEventDetails->setDirection(QBoxLayout::TopToBottom);
    mLayoutEventDetails->addWidget(mEventDetails);
    ui->gb_Details->setLayout(mLayoutEventDetails);

    QObject::connect(ui->btn_DebProfil,&QPushButton::clicked,this,&EventManager::clickOnDebProfil);
    QObject::connect(ui->btn_FinProfil,&QPushButton::clicked,this,&EventManager::clickOnFinProfil);
    QObject::connect(ui->btn_Sippican,&QPushButton::clicked,this,&EventManager::clickOnSippican);

    QObject::connect(ui->btn_NewEvent,&QPushButton::clicked,this,&EventManager::clickOnNewEvent);
    QObject::connect(ui->btn_startAcqui,&QPushButton::clicked,this,&EventManager::clickOnDebAcquisition);
    QObject::connect(ui->btn_StopAcqui,&QPushButton::clicked,this,&EventManager::clickOnFinAcquisition);

    QObject::connect(ui->btn_MajData,&QPushButton::clicked,this,&EventManager::clickOnMajDatas);
    QObject::connect(mEventDetails,&EventDetails::addEvent,this,&EventManager::eventAdded);
    QObject::connect(mEventDetails,&EventDetails::editingFinished,this,&EventManager::addEventFinished);

    ui->sw_Details->setCurrentIndex(0);

}

EventManager::~EventManager()
{
    delete ui;
}

void EventManager::initManager(EventSettings::eventProperties prop)
{
    mEventType=prop;
   ui->sw_Event->setCurrentIndex(mEventType.typeId);


   ui->sw_MajData->setCurrentIndex(0);
   ui->l_EventName->setText(prop.name);
   mProfilStatus=false; //A mettre en QSettings
   mAcquisitionStatus=false;//A mettre en QSettings
   mCurrentLog.type=mEventType.typeName;
   mCurrentLog.typeId=mEventType.typeId;
   mCurrentLog.equipement=mEventType.name;
   mEventDetails->initEvent(mCurrentLog);

   this->hide();
}

QString EventManager::getName()
{
    return mEventType.name;
}

void EventManager::buttonIsChecked(bool bStatus)
{
    if(bStatus)
        this->show();
    else
        this->hide();
}

void EventManager::majCurrentGPS(NavData::stGPSDatas dataGPS)
{
    mCurrentSensors.GPS=dataGPS;
}

void EventManager::majCurrentSBE(NavData::stSBEDatas dataSBE)
{
    mCurrentSensors.SBE=dataSBE;
}

void EventManager::majCurrentDepth(double dSonde)
{

    mCurrentSensors.depth=dSonde;
}

void EventManager::majCurrentCelerimeter(double dCelerimetre)
{
    mCurrentSensors.celerimeter=dCelerimetre;
}

void EventManager::majLastEvent(LogbookModel::stLogbookData lastEvent)
{
    if(lastEvent.equipement==mCurrentLog.equipement)

    {
        if(!mEdition)
        {
        mCurrentLog=lastEvent;
        mEventDetails->initEvent(mCurrentLog);
        majStatus();
        }

    }

}

void EventManager::clickOnDebProfil()
{
   // ui->sw_profil->setCurrentIndex(1);
    //ui->sp_NumProfil->setValue(ui->sp_NumProfil->value()+1);
   mCurrentLog.titre="Début de profil";
   majDetails();
  // mProfilStatus=true;
   ui->btn_DebProfil->setEnabled(!mProfilStatus);
   ui->btn_FinProfil->setEnabled(mProfilStatus);
    mEdition=true;

}

void EventManager::clickOnFinProfil()
{
    mCurrentLog.titre="Fin de profil";
    majDetails();
   // mProfilStatus=false;
    ui->btn_DebProfil->setEnabled(!mProfilStatus);
    ui->btn_FinProfil->setEnabled(mProfilStatus);
    mEdition=true;


}

void EventManager::clickOnSippican()
{
   mCurrentLog.titre="Tir Sippican";
   mCurrentLog.sisLoaded=false;
   mCurrentLog.dateheureSIS.setDate(QDate(2000,01,01));
   mCurrentLog.dateheureSIS.setTime(QTime(0,0,0));
   mCurrentLog.celSippican=1500;

   majDetails();
   mEdition=true;
}



void EventManager::clickOnNewEvent()
{
  mCurrentLog.titre="Evénement";
    majDetails();
    mEdition=true;
}

void EventManager::clickOnDebAcquisition()
{
    mCurrentLog.titre="Début acquisition";
    majDetails();
   // mAcquisitionStatus=true;
    ui->btn_startAcqui->setEnabled(!mAcquisitionStatus);
    ui->btn_StopAcqui->setEnabled(mAcquisitionStatus);
    mEdition=true;
}

void EventManager::clickOnFinAcquisition()
{
    mCurrentLog.titre="Fin d'acquisition";
    majDetails();
   // mAcquisitionStatus=false;
    ui->btn_startAcqui->setEnabled(!mAcquisitionStatus);
    ui->btn_StopAcqui->setEnabled(mAcquisitionStatus);
    mEdition=true;
}


void EventManager::clickOnMajDatas()
{

    ui->sw_MajData->setCurrentIndex(1);
    // mEventDetails->initEvent(mCurrentLog);
    mSensorsRetenus=mCurrentSensors;
    mEventDetails->majSensorsData(mSensorsRetenus);

    mCurrentLog.dateheure=mSensorsRetenus.GPS.dateheure;
    mCurrentLog.cog=mSensorsRetenus.GPS.COG;
    mCurrentLog.sog=mSensorsRetenus.GPS.SOG;
    mCurrentLog.position= mSensorsRetenus.GPS.position;
    mCurrentLog.salinite=mSensorsRetenus.SBE.salinite;
    mCurrentLog.celSBE=mSensorsRetenus.SBE.celerite;
    mCurrentLog.tempSBE=mSensorsRetenus.SBE.tempExt;
    mCurrentLog.sonde=mSensorsRetenus.depth;
    mCurrentLog.celerimetre=mSensorsRetenus.celerimeter;


    mEventDetails->lock(false);

}



void EventManager::eventAdded(LogbookModel::stLogbookData stEvent)
{
    mEdition=false;
    emit addEvent(stEvent);
}

void EventManager::addEventFinished(bool bValid)
{
    mEdition=false;
    ui->sw_Details->setCurrentIndex(0);
    ui->sw_MajData->setCurrentIndex(0);
    if(bValid)
    {

        majStatus();
    }
    emit setButtonChecked(false);

    this->hide();

}

void EventManager::majDetails()
{
    ui->sw_Details->setCurrentIndex(1);
    clickOnMajDatas();

    mEventDetails->ajouterEvent(mCurrentLog);


}

void EventManager::majStatus()
{
    if(mCurrentLog.typeId==LogbookModel::Profil)
    {
        if(mCurrentLog.titre=="Début de profil")
        {
            mProfilStatus=true;
            ui->btn_DebProfil->setEnabled(!mProfilStatus);
            ui->btn_FinProfil->setEnabled(mProfilStatus);
        }
        else{
            mProfilStatus=false;
            ui->btn_DebProfil->setEnabled(!mProfilStatus);
            ui->btn_FinProfil->setEnabled(mProfilStatus);
        }

    }
    if(mCurrentLog.typeId==LogbookModel::ArcContinu)
    {
        if(mCurrentLog.titre=="Début acquisition")
        {
            mAcquisitionStatus=true;
            ui->btn_startAcqui->setEnabled(!mAcquisitionStatus);
            ui->btn_StopAcqui->setEnabled(mAcquisitionStatus);
        }
        else
        {
            mAcquisitionStatus=false;
            ui->btn_startAcqui->setEnabled(!mAcquisitionStatus);
            ui->btn_StopAcqui->setEnabled(mAcquisitionStatus);
        }

    }
}




