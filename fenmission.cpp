#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QDesktopServices>
//#include <QListIterator>

#include "fenmission.h"
#include "ui_fenmission.h"

fenMission::fenMission(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fenMission)
{
    ui->setupUi(this);

    mSettings=new QSettings();
    QObject::connect(ui->btn_Valider,&QPushButton::clicked,this,&fenMission::valider);
    QObject::connect(ui->btn_Annuler,&QPushButton::clicked,this,&fenMission::annuler);
    QObject::connect(ui->cb_Admin,&QCheckBox::stateChanged,this,&fenMission::setAdmin);
    mCurrentCruise.sNom=mSettings->value("Mission/Nom","").toString();
    mCurrentCruise.sZone=mSettings->value("Mission/Zone","").toString();
    mCurrentCruise.sNavire=mSettings->value("Mission/Navire","").toString();
    mCurrentCruise.sChefMission=mSettings->value("Mission/ChefMission","").toString();
    mCurrentCruise.dateDebut=mSettings->value("Mission/DateDebut","").toDate();
    mCurrentCruise.dateFin=mSettings->value("Mission/DateFin","").toDate();
    mCurrentCruise.sOperateurs=mSettings->value("Mission/Operateurs","").toString();


}

fenMission::~fenMission()
{
    delete ui;
}

fenMission::st_Mission fenMission::getCurrentCruise()
{
    return mCurrentCruise;
  /*  st_Mission currentCruise;
    currentCruise.sNom=mSettings->value("Mission/Nom","").toString();
    currentCruise.sZone=mSettings->value("Mission/Zone","").toString();
    currentCruise.sNavire=mSettings->value("Mission/Navire","").toString();
    currentCruise.sChefMission=mSettings->value("Mission/ChefMission","").toString();
    currentCruise.dateDebut=mSettings->value("Mission/DateDebut","").toDate();
    currentCruise.dateFin=mSettings->value("Mission/DateFin","").toDate();
    currentCruise.sOperateurs=mSettings->value("Mission/Operateurs","").toString();
    //currentCruise.listDataGroups=mSettings->value("Mission/DataGroups","").toStringList();
    //currentCruise.sPath=mSettings->value("Mission/Path","").toString();

    return currentCruise;*/
}

void fenMission::newCruise()
{
    ui->le_Mission->setText("");
    ui->le_Zone->setText("");
    ui->le_Navire->setText("");
    ui->le_ChefMission->setText("");
    ui->te_Operateurs->clear();
    ui->de_Debut->setDate(QDate::currentDate());
    ui->de_Fin->setDate(QDate::currentDate().addDays(7));

    mNewCruise=true;
    ui->cb_Admin->setChecked(true);

    setAdmin(Qt::Checked);
    this->show();
}

void fenMission::editCruise()
{

    st_Mission currentCruise=getCurrentCruise();
    ui->le_Mission->setText(currentCruise.sNom);
    ui->le_Zone->setText(currentCruise.sZone);
    ui->le_Navire->setText(currentCruise.sNavire);
    ui->le_ChefMission->setText(currentCruise.sChefMission);
    ui->te_Operateurs->setText(currentCruise.sOperateurs);
    ui->de_Debut->setDate(currentCruise.dateDebut);
    ui->de_Fin->setDate(currentCruise.dateFin);

    mNewCruise=false;
    ui->cb_Admin->setChecked(false);

    setAdmin(Qt::Unchecked);
    this->show();
}

QString fenMission::getCurrentCruiseName()
{
    return mCurrentCruise.sNom;
    //return mSettings->value("Mission/Nom","").toString();
}

void fenMission::setCurrentCruise(fenMission::st_Mission currentCruise)
{
    mCurrentCruise=currentCruise;
    mSettings->setValue("Mission/Nom",currentCruise.sNom);
    mSettings->setValue("Mission/Zone",currentCruise.sZone);
    mSettings->setValue("Mission/Navire",currentCruise.sNavire);
    mSettings->setValue("Mission/ChefMission",currentCruise.sChefMission);
    mSettings->setValue("Mission/DateDebut",currentCruise.dateDebut);
    mSettings->setValue("Mission/DateFin",currentCruise.dateFin);
    mSettings->setValue("Mission/Operateurs",currentCruise.sOperateurs);
}



void fenMission::valider()
{
    if (ui->le_Mission->text().isEmpty())
    {
        QMessageBox::warning(this,QString("Erreur lors de la création de campagne"),QString("Le nom de la mission ne peut pas être vide"));
        return;
    }


    st_Mission currentCruise;
    currentCruise.sNom=ui->le_Mission->text();
    currentCruise.sZone=ui->le_Zone->text();
    currentCruise.sNavire=ui->le_Navire->text();
    currentCruise.sOperateurs=ui->te_Operateurs->toPlainText();
    currentCruise.sChefMission=ui->le_ChefMission->text();
    currentCruise.dateDebut=ui->de_Debut->date();
    currentCruise.dateFin=ui->de_Fin->date();

    mSettings->setValue("Mission/Nom",currentCruise.sNom);
    mSettings->setValue("Mission/Zone",currentCruise.sZone);
    mSettings->setValue("Mission/Navire",currentCruise.sNavire);
    mSettings->setValue("Mission/ChefMission",currentCruise.sChefMission);
    mSettings->setValue("Mission/DateDebut",currentCruise.dateDebut);
    mSettings->setValue("Mission/DateFin",currentCruise.dateFin);
    mSettings->setValue("Mission/Operateurs",currentCruise.sOperateurs);

    if(mNewCruise)
    {      
        emit newCruiseSet(currentCruise.sNom);

    }

    emit newCruiseDetails(currentCruise);
    emit editingFinished();

    mCurrentCruise=currentCruise;

    this->close();
}

void fenMission::annuler()
{
    this->close();
}

void fenMission::setAdmin(int nAdm)
{
    bool bAdm=false;
    if(nAdm==Qt::Checked)
        bAdm=true;
    ui->le_Zone->setReadOnly(!bAdm);
    ui->le_Navire->setReadOnly(!bAdm);
    ui->le_Mission->setReadOnly(!bAdm);
    ui->le_ChefMission->setReadOnly(!bAdm);
    ui->te_Operateurs->setReadOnly(!bAdm);
    ui->de_Debut->setReadOnly(!bAdm);
    ui->de_Fin->setReadOnly(!bAdm);


}



