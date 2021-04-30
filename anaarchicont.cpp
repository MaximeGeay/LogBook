#include "anaarchicont.h"
#include "ui_anaarchicont.h"

AnaArchiCont::AnaArchiCont(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnaArchiCont)
{
    ui->setupUi(this);
}

AnaArchiCont::~AnaArchiCont()
{
    delete ui;
}

void AnaArchiCont::setName(QString sName)
{
     ui->gb_Name->setTitle(sName);
}

void AnaArchiCont::setDuree(QString sDuree)
{
    ui->l_Duree->setText(sDuree);
}
