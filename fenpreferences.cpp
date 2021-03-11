#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>

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
    QObject::connect(ui->btn_SelConfigFile,&QToolButton::clicked,this,&FenPreferences::clickOnSelFicConfig);
    QObject::connect(ui->btn_SelLogbookDir,&QToolButton::clicked,this,&FenPreferences::clickOnSelRepLogbook);
    QObject::connect(this,&FenPreferences::xmlError,this,&FenPreferences::showErrors);
    QObject::connect(mEventSettings,&EventSettings::addEvent,mEventListModel,&EventListModel::appendEvent);
    QObject::connect(mEventSettings,&EventSettings::eventChanged,mEventListModel,&EventListModel::modifyElement);
    QObject::connect(mEventListModel,&EventListModel::eventAlreadyExist,mEventSettings,&EventSettings::eventIsInvalid);

    QSettings settings;
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!dir.exists())
        dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    mPathToConfXML=settings.value("PathToConfXML",QString("%1/config_logbook.xml").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))).toString();
    mPathToLogbookDir=settings.value("PathToLogbookDir",QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))).toString();
    mEventListModel->initXML(mPathToConfXML);
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
    ui->le_ConfigXML->setText(mPathToConfXML);


    this->show();
}

QList<EventSettings::eventProperties> FenPreferences::getEventTypeList()
{
    return mEventListModel->getEventList();
}

void FenPreferences::openRepConf()
{
    int n = mPathToConfXML.count("/");
    QDesktopServices::openUrl(mPathToConfXML.section("/",0,n-1));
}

QString FenPreferences::getLogbookDir()
{
    return mPathToLogbookDir;
}

void FenPreferences::clickOnValider()
{
    QSettings settings;
    settings.setValue("PortGPS",ui->sp_PortGPS->value());
    if(mPathToConfXML!=ui->le_ConfigXML->text())
    {
        QString sPath=ui->le_ConfigXML->text();
        mEventListModel->initXML(sPath);
        if(QFile::exists(sPath))
        {
            mPathToConfXML=sPath;
            settings.setValue("PathToConfXML",mPathToConfXML);
        }

    }
    if(mPathToLogbookDir!=ui->le_LogbookDir->text())
    {
       QDir sDir=QDir(ui->le_LogbookDir->text());
       if(sDir.exists(ui->le_LogbookDir->text()))
       {
           mPathToLogbookDir=ui->le_LogbookDir->text();
           settings.setValue("PathToLogbookDir",mPathToLogbookDir);
       }
    }

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

void FenPreferences::clickOnSelFicConfig()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Fichier de configuration XML"),
                               mPathToConfXML,
                               tr("XML files (*.xml)"));

    if(!fileName.isEmpty())
        ui->le_ConfigXML->setText(fileName);

}

void FenPreferences::clickOnSelRepLogbook()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Répertoire du cahier de quart XML"),
                               mPathToLogbookDir);

    if(!dirPath.isEmpty())
        ui->le_LogbookDir->setText(dirPath);
}

void FenPreferences::majListEvent()
{


}

void FenPreferences::showErrors(QString sError)
{
    emit xmlError(sError);
}


