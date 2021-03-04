#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

#include "fenpreferences.h"
#include "ui_fenpreferences.h"


FenPreferences::FenPreferences(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FenPreferences)
{
    ui->setupUi(this);

    mEventSettings=new EventSettings;
    mEventListModel=new EventListModel;

    QObject::connect(ui->btn_Valider,&QPushButton::clicked,this,&FenPreferences::clickOnValider);
    QObject::connect(ui->btn_Annuler,&QPushButton::clicked,this,&FenPreferences::clickOnAnnuler);
    QObject::connect(ui->btn_AddEvent,&QPushButton::clicked,this,&FenPreferences::clickOnAddEvent);
    QObject::connect(ui->btn_ModifyEvent,&QPushButton::clicked,this,&FenPreferences::clickOnModifyEvent);
    QObject::connect(ui->btn_RemoveEvent,&QPushButton::clicked,this,&FenPreferences::clickOnRemoveEvent);
    QObject::connect(this,&FenPreferences::xmlError,this,&FenPreferences::showErrors);
    QObject::connect(mEventSettings,&EventSettings::addEvent,mEventListModel,&EventListModel::appendEvent);
    QObject::connect(mEventSettings,&EventSettings::eventChanged,mEventListModel,&EventListModel::modifyElement);
    QObject::connect(mEventListModel,&EventListModel::eventAlreadyExist,mEventSettings,&EventSettings::eventIsInvalid);

    mPathToXML="E:/test.xml";
    mEventListModel->initXML(mPathToXML);
    ui->tb_ListEvent->setModel(mEventListModel);


}

FenPreferences::~FenPreferences()
{
    delete ui;
}

void FenPreferences::initFen()
{
    QSettings settings;
    int nPortGps=settings.value("PortGPS",20005).toInt();
    ui->sp_PortGPS->setValue(nPortGps);

    this->show();
}

QList<EventSettings::eventProperties> FenPreferences::getEventTypeList()
{
    return mEventListModel->getEventList();
}

void FenPreferences::clickOnValider()
{
    QSettings settings;
    settings.setValue("PortGPS",ui->sp_PortGPS->value());
    emit confChanged();
    clickOnAnnuler();

}

void FenPreferences::clickOnAnnuler()
{
    this->close();
}

void FenPreferences::clickOnAddEvent()
{
    mEventSettings->newEvent();
}

void FenPreferences::clickOnModifyEvent()
{
    QString sEvent;
    if(ui->tb_ListEvent->currentIndex().column()>0)
        sEvent=ui->tb_ListEvent->currentIndex().siblingAtColumn(0).data().toString();
    else
    {
        sEvent=ui->tb_ListEvent->currentIndex().data().toString();
    }

   if(mEventListModel->eventNameExist(sEvent))
   {
       int bRes=QMessageBox::question(this,"Modification d'un type d'événement",QString("Etes-vous sûr de vouloir modifier le type d'événement %1?\n"
                                                                                       "Cette action est irreversible.").arg(sEvent),QMessageBox::Yes|QMessageBox::No);
       if(bRes==QMessageBox::Yes)
       {
           mEventSettings->modifyEvent(mEventListModel->findEvent(sEvent));

       }
   }
}

void FenPreferences::clickOnRemoveEvent()
{
    QString sEvent;
    if(ui->tb_ListEvent->currentIndex().column()>0)
        sEvent=ui->tb_ListEvent->currentIndex().siblingAtColumn(0).data().toString();
    else
    {
        sEvent=ui->tb_ListEvent->currentIndex().data().toString();
    }

   if(mEventListModel->eventNameExist(sEvent))
   {
       int bRes=QMessageBox::question(this,"Suppression d'un type d'événement",QString("Etes-vous sûr de vouloir supprimer le type d'événement %1?\n"
                                                                                       "Cette action est irreversible.").arg(sEvent),QMessageBox::Yes|QMessageBox::No);
       if(bRes==QMessageBox::Yes)
       {
           mEventListModel->removeElement(mEventListModel->findEvent(sEvent));
       }
   }
}

void FenPreferences::majListEvent()
{


}

void FenPreferences::showErrors(QString sError)
{
    qDebug()<<sError;
}


