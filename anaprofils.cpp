#include "anaprofils.h"
#include "ui_anaprofils.h"

AnaProfils::AnaProfils(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnaProfils)
{
    ui->setupUi(this);
}

AnaProfils::~AnaProfils()
{
    delete ui;
}

void AnaProfils::setName(QString sName)
{
    ui->gb_Name->setTitle(sName);
}

void AnaProfils::setDuree(QString sDuree)
{
    ui->l_Duree->setText(sDuree);
}

void AnaProfils::setNombre(QString sNombre)
{
    ui->l_Nombre->setText(sNombre);
}

void AnaProfils::setDistance(QString sDistance)
{
    ui->l_Longueur->setText(sDistance);
}

QString AnaProfils::getName()
{
    return ui->gb_Name->title();
}
