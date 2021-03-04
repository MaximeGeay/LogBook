#include<QMessageBox>

#include "eventsettings.h"
#include "ui_eventsettings.h"

EventSettings::EventSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EventSettings)
{
    ui->setupUi(this);

    QObject::connect(ui->btn_Valider,&QPushButton::clicked,this,&EventSettings::clickOnValider);
    QObject::connect(ui->btn_Annuler,&QPushButton::clicked,this,&EventSettings::clickOnAnnuler);
}

EventSettings::~EventSettings()
{
    delete ui;
}

void EventSettings::eventIsInvalid(bool bExist)
{
    if(bExist)
    {
     QMessageBox::warning(this,"Nom existant","Cet évènement existe déjà veuillez choisir un autre nom");
    }
    else
    {
        this->close();
    }
}

void EventSettings::newEvent()
{
    ui->le_Name->setText("");
    ui->cb_Type->setCurrentIndex(0);
    mIsNew=true;

    this->show();
}

void EventSettings::modifyEvent(EventSettings::eventProperties prop)
{
    ui->le_Name->setText(prop.name);
    ui->cb_Type->setCurrentIndex(prop.typeId);
    mIsNew=false;
    mCurrentProperties=prop;
    this->show();
}

void EventSettings::clickOnValider()
{
    eventProperties prop;
    prop.name=ui->le_Name->text();
    prop.typeId=ui->cb_Type->currentIndex();
    prop.typeName=ui->cb_Type->currentText();

    if(mIsNew)
        emit addEvent(prop);
    else
        emit eventChanged(mCurrentProperties,prop);

    mCurrentProperties=prop;



}

void EventSettings::clickOnAnnuler()
{
    this->close();
}
