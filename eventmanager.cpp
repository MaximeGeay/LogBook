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
    QObject::connect(ui->btn_Valider,&QPushButton::clicked,this,&EventManager::clickOnValider);
    QObject::connect(ui->btn_Annuler,&QPushButton::clicked,this,&EventManager::clickOnAnnuler);



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
   ui->sw_DataRetenues->setCurrentIndex(0);
   ui->l_EventName->setText(prop.name);


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
    writeDatas();


}

void EventManager::clickOnFinProfil()
{
    clickOnDebProfil();
}

void EventManager::clickOnAnnuler()
{
    ui->sw_DataRetenues->setCurrentIndex(0);
    //this->hide();
   //emit setButtonChecked(false);
}

void EventManager::clickOnValider()
{
    QDomDocument dom;
    QDomElement element=dom.createElement("Event");
    element.setAttribute("Equipement",mEventType.name);
    element.setAttribute("Type",mEventType.typeName);
    element.setAttribute("DateHeure",ui->sp_dtRetenue->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    element.setAttribute("Position",ui->le_PosRetenue->text());
    element.setAttribute("COG",QString(ui->sp_COGRetenu->value()));
    element.setAttribute("SOG",QString(ui->sp_SOGRetenu->value()));


    if(mEventType.typeId==1) //Profil
    {
        if(mProfilStatus)
        {
            element.setAttribute("Titre","Début de profil");
        }
        else
        {
            element.setAttribute("Titre","Fin de profil");
        }
        element.setAttribute("Fichier",ui->le_fichier->text());
        element.setAttribute("Profil",QString(ui->sp_NumProfil->value()));
        element.setAttribute("Commentaires",ui->te_CommentsProfil->toPlainText());


        mProfilStatus=!mProfilStatus;
        ui->btn_DebProfil->setEnabled(!mProfilStatus);
        ui->btn_FinProfil->setEnabled(mProfilStatus);
    }

    if(mEventType.typeId==2)//Sippican
    {

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

    }



    emit addEvent(element);
    clickOnAnnuler();

}

void EventManager::writeDatas()
{
    ui->sw_DataRetenues->setCurrentIndex(1);
    ui->sp_dtRetenue->setDateTime(mCurrentData.dateheure);
    ui->sp_COGRetenu->setValue(mCurrentData.COG);
    ui->sp_SOGRetenu->setValue(mCurrentData.SOG);
    ui->le_PosRetenue->setText(mCurrentData.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere));

}



