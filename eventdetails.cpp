#include<QMessageBox>
#include <QGeoCoordinate>

#include "eventdetails.h"
#include "ui_eventdetails.h"

EventDetails::EventDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventDetails)
{
    ui->setupUi(this);
    QObject::connect(ui->btn_UnLock,&QToolButton::clicked,this,&EventDetails::clickOnUnlock);
    QObject::connect(ui->btn_Lock,&QToolButton::clicked,this,&EventDetails::clickOnLock);
    QObject::connect(ui->btn_Modifier,&QPushButton::clicked,this,&EventDetails::clickOnModifier);
    QObject::connect(ui->btn_Supprimer,&QPushButton::clicked,this,&EventDetails::clickOnSupprimer);
    QObject::connect(ui->btn_Valider,&QPushButton::clicked,this,&EventDetails::clickOnValider);
    QObject::connect(ui->btn_Annuler,&QPushButton::clicked,this,&EventDetails::clickOnAnnuler);
    QObject::connect(ui->cb_ChargeSIS,&QCheckBox::clicked,this,&EventDetails::SISChecked);
    lock(true);
}

EventDetails::~EventDetails()
{
    delete ui;
}

void EventDetails::initEvent(LogbookModel::stLogbookData event)
{

    ui->l_Equipement->setText(event.equipement);
    ui->l_Type->setText(event.type);
    ui->le_Titre->setText(event.titre);
    ui->te_Comments->setText(event.commentaire);
    ui->le_Position->setText(event.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere));
    ui->sp_COG->setValue(event.cog);
    ui->sp_SOG->setValue(event.sog);
    ui->sp_Sonde->setValue(event.sonde);
    ui->sp_SBETemp->setValue(event.tempSBE);
    ui->sp_SBECelerite->setValue(event.celSBE);
    ui->sp_SBESalinite->setValue(event.salinite);
    ui->sp_Celerimetre->setValue(event.celerimetre);
    ui->dt_Event->setDateTime(event.dateheure);
    ui->le_FicSis->setText(event.ficSIS);
    ui->le_FicProfil->setText(event.ficProfil);
    ui->sp_NumProfil->setValue(event.profil);
    ui->le_FicASVP->setText(event.ficASVP);
    ui->le_probeType->setText(event.typeSippican);
    ui->le_ProbeNumber->setValue(event.probeNumber);
    ui->sp_CelASVP->setValue(event.celSippican);

    ui->dt_ChargeSIS->setDateTime(event.dateheureSIS);
    ui->cb_ChargeSIS->setChecked(event.sisLoaded);
    ui->cb_RefCelerite->setCurrentIndex(event.refCelerite);

    if(event.typeId==LogbookModel::Profil||event.typeId==LogbookModel::Sippican)
        ui->sw_Sip_Pro->setCurrentIndex(event.typeId);
    else
        ui->sw_Sip_Pro->setCurrentIndex(0);

    if(mAjout)
    {
        QPalette *palette = new QPalette();
        palette->setColor(QPalette::Base,Qt::yellow);

        ui->le_FicProfil->setPalette(*palette);
        ui->sp_NumProfil->setPalette(*palette);
        ui->sp_CelASVP->setPalette(*palette);
        ui->le_FicASVP->setPalette(*palette);
        ui->le_ProbeNumber->setPalette(*palette);
        ui->le_FicSis->setPalette(*palette);
        ui->dt_ChargeSIS->setPalette(*palette);
    }
    mCurrentEvent=event;
    lock(true);

}

void EventDetails::ajouterEvent(LogbookModel::stLogbookData stEvent)
{
    mAjout=true;
    initEvent(stEvent);

    ui->sw_btns->setCurrentIndex(2);
    lock(false);

}

void EventDetails::clickOnUnlock()
{
   ui->sw_btns->setCurrentIndex(1);
}

void EventDetails::clickOnLock()
{
    ui->sw_btns->setCurrentIndex(0);
}

void EventDetails::clickOnModifier()
{
    lock(false);
    ui->sw_btns->setCurrentIndex(2);

}

void EventDetails::clickOnSupprimer()
{
    int res=QMessageBox::question(this,"Suppression d'un événement","Etes-vous sûr de vouloir supprimer cet événement?",QMessageBox::Yes|QMessageBox::No);
    if(res==QMessageBox::Yes)
        emit removeEvent(mCurrentEvent);
    clickOnAnnuler();

}


void EventDetails::clickOnValider()
{

    if(!mAjout)
    {

    int res=QMessageBox::question(this,"Modification d'un événement","Etes-vous sûr de vouloir modifier cet événement?",QMessageBox::Yes|QMessageBox::No);
    if(res==QMessageBox::No)
        return;
    }

    QGeoCoordinate pos(stringToPos(ui->le_Position->text()));
    if(!pos.isValid())
    {
        QMessageBox::warning(this,"Position invalide",QString("La position %1 n'a pas un format valide\n"
                                                              "Le format doit être : DD° MM.ddd H', DDD° MM.ddd' H").arg(ui->le_Position->text()));
        ui->le_Position->setText(mCurrentEvent.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere));
        return;

    }

    mCurrentEvent.titre=ui->le_Titre->text();
    mCurrentEvent.commentaire=ui->te_Comments->toPlainText();
   mCurrentEvent.cog=ui->sp_COG->value();
   mCurrentEvent.sog=ui->sp_SOG->value();
   mCurrentEvent.sonde=ui->sp_Sonde->value();
   mCurrentEvent.tempSBE=ui->sp_SBETemp->value();
   mCurrentEvent.celSBE=ui->sp_SBECelerite->value();
   mCurrentEvent.salinite=ui->sp_SBESalinite->value();
   mCurrentEvent.celerimetre=ui->sp_Celerimetre->value();
   mCurrentEvent.dateheure=ui->dt_Event->dateTime();
   mCurrentEvent.ficSIS=ui->le_FicSis->text();
   mCurrentEvent.ficProfil=ui->le_FicProfil->text();
   mCurrentEvent.profil=ui->sp_NumProfil->value();
   mCurrentEvent.ficASVP=ui->le_FicASVP->text();
   mCurrentEvent.typeSippican=ui->le_probeType->text();
   mCurrentEvent.probeNumber=ui->le_ProbeNumber->value();
   mCurrentEvent.celSippican=ui->sp_CelASVP->value();
   mCurrentEvent.dateheureSIS=ui->dt_ChargeSIS->dateTime();
   mCurrentEvent.sisLoaded=ui->cb_ChargeSIS->isChecked();
   mCurrentEvent.refCelerite=ui->cb_RefCelerite->currentIndex();

    mValidation=true;
   if(mAjout)
   {

       emit addEvent(mCurrentEvent);
   }
   else
       emit modifyEvent(mCurrentEvent);



   clickOnAnnuler();



}

void EventDetails::clickOnAnnuler()
{
    initEvent(mCurrentEvent);
    ui->sw_btns->setCurrentIndex(0);
    mAjout=false;
    emit editingFinished(mValidation);
    mValidation=false;


}

void EventDetails::SISChecked(bool bCheck)
{
    if(ui->cb_ChargeSIS->isChecked())
        ui->dt_ChargeSIS->setDateTime(QDateTime::currentDateTimeUtc());
}

void EventDetails::majSensorsData(NavData::stSensorsDatas sensors)
{
    ui->sp_COG->setValue(sensors.GPS.COG);
    ui->sp_SOG->setValue(sensors.GPS.SOG);
    ui->le_Position->setText(sensors.GPS.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere));
    ui->sp_Sonde->setValue(sensors.depth);
    ui->sp_Celerimetre->setValue(sensors.celerimeter);
    ui->sp_SBETemp->setValue(sensors.SBE.tempExt);
    ui->sp_SBECelerite->setValue(sensors.SBE.celerite);
    ui->sp_SBESalinite->setValue(sensors.SBE.salinite);
    ui->dt_Event->setDateTime(sensors.GPS.dateheure);
}

void EventDetails::lock(bool bLock)
{

    ui->le_Titre->setReadOnly(bLock);
    ui->te_Comments->setReadOnly(bLock);
    ui->le_Position->setReadOnly(bLock);
    ui->sp_COG->setReadOnly(bLock);
    ui->sp_SOG->setReadOnly(bLock);
    ui->sp_Sonde->setReadOnly(bLock);
    ui->sp_SBETemp->setReadOnly(bLock);
    ui->sp_SBECelerite->setReadOnly(bLock);
    ui->sp_SBESalinite->setReadOnly(bLock);
    ui->sp_Celerimetre->setReadOnly(bLock);
    ui->dt_Event->setReadOnly(bLock);
    ui->le_FicSis->setReadOnly(bLock);
    ui->le_FicProfil->setReadOnly(bLock);
    ui->sp_NumProfil->setReadOnly(bLock);
    ui->le_FicASVP->setReadOnly(bLock);
    ui->le_probeType->setReadOnly(bLock);
    ui->dt_ChargeSIS->setReadOnly(bLock);
    ui->cb_ChargeSIS->setEnabled(!bLock);
    ui->cb_RefCelerite->setEnabled(!bLock);
    ui->le_ProbeNumber->setReadOnly(bLock);
    ui->sp_CelASVP->setReadOnly(bLock);



}

QGeoCoordinate EventDetails::stringToPos(QString sPos)
{
    //47° 37.264' N, 1° 18.254' E
    QString sLat=sPos.section(",",0,0);
    QString sLong=sPos.section(",",1,1);
    QGeoCoordinate geo(latMinToDec(sLat),longMinToDec(sLong));

    return geo;
}

double EventDetails::latMinToDec(QString sLatitude)
{
    //format : DD° MM.ddd' H
    //         47° 37.264' N
        int nDeg=sLatitude.section("°",0,0).toInt();
        double dMin=sLatitude.section("°",1,1).section("'",0,0).remove(" ").toDouble();
        QString cHemi=sLatitude.section("'",1,1).remove(" ");

        double dLatDec=nDeg+(dMin/60);
        if(cHemi=="S")
            dLatDec=-dLatDec;

        return dLatDec;
}

double EventDetails::longMinToDec(QString sLongitude)
{
    //format :DD° MM.ddd' H
    //        1° 18.254' E
    int nDeg=sLongitude.section("°",0,0).toInt();
    double dMin=sLongitude.section("°",1,1).section("'",0,0).remove(" ").toDouble();
    QString cHemi=sLongitude.section("'",1,1).remove(" ");

    double dLongDec=nDeg+(dMin/60);
    if(cHemi=="W")
        dLongDec=-dLongDec;

    return dLongDec;
}
