#include <QMessageBox>
#include <QDebug>

#include "eventmanager.h"
#include "ui_eventmanager.h"

EventManager::EventManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventManager)
{
    ui->setupUi(this);

    QObject::connect(ui->btn_DebProfil,&QPushButton::clicked,this,&EventManager::clickOnDebProfil);
    QObject::connect(ui->btn_FinProfil,&QPushButton::clicked,this,&EventManager::clickOnFinProfil);
    QObject::connect(ui->btn_Sippican,&QPushButton::clicked,this,&EventManager::clickOnSippican);
    QObject::connect(ui->btn_LoadSip,&QPushButton::clicked,this,&EventManager::clickOnLoadSip);
    QObject::connect(ui->btn_NewEvent,&QPushButton::clicked,this,&EventManager::clickOnNewEvent);
    QObject::connect(ui->btn_startAcqui,&QPushButton::clicked,this,&EventManager::clickOnDebAcquisition);
    QObject::connect(ui->btn_StopAcqui,&QPushButton::clicked,this,&EventManager::clickOnFinAcquisition);
    QObject::connect(ui->btn_Valider,&QPushButton::clicked,this,&EventManager::clickOnValider);
    QObject::connect(ui->btn_Annuler,&QPushButton::clicked,this,&EventManager::clickOnAnnuler);
    QObject::connect(ui->btn_MajData,&QPushButton::clicked,this,&EventManager::clickOnMajDatas);



}

EventManager::~EventManager()
{
    delete ui;
}

void EventManager::initManager(EventSettings::eventProperties prop)
{
    mEventType=prop;
   ui->sw_Event->setCurrentIndex(mEventType.typeId);
   ui->sw_profil->setCurrentIndex(0);
   ui->sw_Sippican->setCurrentIndex(0);
   ui->sw_NewEvent->setCurrentIndex(0);
   ui->sw_DataRetenues->setCurrentIndex(0);
   ui->l_EventName->setText(prop.name);
   mProfilStatus=false; //A mettre en QSettings
   mAcquisitionStatus=false;//A mettre en QSettings


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

void EventManager::majCurrentData(NavData::Datas datas)
{
    mCurrentData=datas;
}

void EventManager::clickOnDebProfil()
{
    ui->sw_profil->setCurrentIndex(1);
    ui->sp_NumProfil->setValue(ui->sp_NumProfil->value()+1);
    clickOnMajDatas();


}

void EventManager::clickOnFinProfil()
{
     ui->sw_profil->setCurrentIndex(1);
    clickOnMajDatas();
}

void EventManager::clickOnSippican()
{
    ui->sw_Sippican->setCurrentIndex(1);
    ui->sp_nbSippican->setValue(ui->sp_nbSippican->value()+1);
    clickOnMajDatas();
}

void EventManager::clickOnLoadSip()
{
    ui->dte_LoadSip->setDateTime(QDateTime::currentDateTimeUtc());
}

void EventManager::clickOnNewEvent()
{
    ui->sw_NewEvent->setCurrentIndex(1);
    clickOnMajDatas();
}

void EventManager::clickOnAnnuler()
{
    ui->sw_DataRetenues->setCurrentIndex(0);
    if(mEventType.typeId==1)
        ui->sw_profil->setCurrentIndex(0);
    if(mEventType.typeId==2)
        ui->sw_Sippican->setCurrentIndex(0);
    if(mEventType.typeId==3)
        ui->sw_NewEvent->setCurrentIndex(0);
    if(mEventType.typeId==4)
        ui->sw_ArcCont->setCurrentIndex(0);

}

void EventManager::clickOnValider()
{
    QDomDocument dom;
    QDomElement element=dom.createElement("Event");
    element.setAttribute("Equipement",mEventType.name);
    element.setAttribute("Type",mEventType.typeName);
    element.setAttribute("DateHeure",ui->sp_dtRetenue->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    element.setAttribute("Position",ui->le_PosRetenue->text());
    element.setAttribute("COG",QString::number(ui->sp_COGRetenu->value()));
    element.setAttribute("SOG",QString::QString::number(ui->sp_SOGRetenu->value()));


    if(mEventType.typeId==1) //Profil
    {
        if(!mProfilStatus)
        {
            element.setAttribute("Titre","Début de profil");
        }
        else
        {
            element.setAttribute("Titre","Fin de profil");
        }
        element.setAttribute("Fichier",ui->le_fichier->text());
        element.setAttribute("Profil",QString::number(ui->sp_NumProfil->value()));
        element.setAttribute("Commentaires",ui->te_CommentsProfil->toPlainText());


        mProfilStatus=!mProfilStatus;
        ui->btn_DebProfil->setEnabled(!mProfilStatus);
        ui->btn_FinProfil->setEnabled(mProfilStatus);
    }

    if(mEventType.typeId==2)//Sippican
    {
        ui->sp_nbSippican->value();
        element.setAttribute("Titre","Tir Sippican");
        element.setAttribute("NumSippican",QString::number(ui->sp_nbSippican->value()));
        element.setAttribute("TypeSippican",ui->le_TypeSonde->text());
        element.setAttribute("FichierSippican",ui->le_FicSippican->text());
        element.setAttribute("TempSBE",QString::number(ui->sp_SBE->value(),'f',1));
        element.setAttribute("Celerimetre",QString::number(ui->sp_Celerimetre->value(),'f',1));
        element.setAttribute("CelSippican",QString::number(ui->sp_CelASVP->value(),'f',1));
        element.setAttribute("ChargeSIS",QString::number(ui->btn_LoadSip->isChecked()));
        element.setAttribute("DateHeureChargeSIS",ui->dte_LoadSip->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
        element.setAttribute("FichierSIS",ui->le_Fic2040->text());
        element.setAttribute("Commentaires",ui->le_CommentSip->text());

    }

    if(mEventType.typeId==3)//Ponctuel
    {
        element.setAttribute("Titre",ui->le_Event->text());
        element.setAttribute("Commentaires",ui->te_CommentEvt->toPlainText());
        ui->le_Event->setText("");
        ui->te_CommentEvt->clear();
    }
    if(mEventType.typeId==4)//Archivage Continu
    {
        if(!mAcquisitionStatus)
            element.setAttribute("Titre","Début acquisition");
        else
            element.setAttribute("Titre","Fin acquisition");

        element.setAttribute("Commentaires",ui->te_CommentArcContinu->toPlainText());

        mAcquisitionStatus=!mAcquisitionStatus;
        ui->btn_startAcqui->setEnabled(!mAcquisitionStatus);
        ui->btn_StopAcqui->setEnabled(mAcquisitionStatus);

    }



    emit addEvent(element);
    clickOnAnnuler();

}

void EventManager::clickOnMajDatas()
{
    ui->sw_DataRetenues->setCurrentIndex(1);
    ui->sp_dtRetenue->setDateTime(mCurrentData.dateheure);
    ui->sp_COGRetenu->setValue(mCurrentData.COG);
    ui->sp_SOGRetenu->setValue(mCurrentData.SOG);
    ui->le_PosRetenue->setText(mCurrentData.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere));

}

void EventManager::clickOnDebAcquisition()
{
    ui->sw_ArcCont->setCurrentIndex(1);
    clickOnMajDatas();
}

void EventManager::clickOnFinAcquisition()
{
    ui->sw_ArcCont->setCurrentIndex(1);
    clickOnMajDatas();
}



