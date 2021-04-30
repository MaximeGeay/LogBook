
#include <QMessageBox>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"


#define version "LogBook 0.3"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(version);
    mFenPref=new FenPreferences();
    mNavData=new NavData();
    mFenMission=new fenMission();
    mLogBookModel=new LogbookModel();
    mEventDetails=new EventDetails();
    mDepouillement=new Depouillement();
    mTableModelProfil=new CustomTableModel(LogbookModel::Profil);
    mTableModelSippican=new CustomTableModel(LogbookModel::Sippican);
    mTableModelSimpleEvent=new CustomTableModel(LogbookModel::Ponctuel);
    mTableModelArcCont=new CustomTableModel(LogbookModel::ArcContinu);


    mLayoutDataManager=new QHBoxLayout();
    mLayoutDataManager->setAlignment(Qt::AlignTop);
    mLayoutDataManager->setDirection(QBoxLayout::LeftToRight);
    mLayoutDataManager->addWidget(mNavData);
    ui->gb_datas->setLayout(mLayoutDataManager);

    mLayoutButtonsManager=new QVBoxLayout();
    mLayoutButtonsManager->setAlignment(Qt::AlignTop);
    mLayoutButtonsManager->setDirection(QBoxLayout::TopToBottom);
    ui->gb_ButtonList->setLayout(mLayoutButtonsManager);

    mLayoutEventManager=new QVBoxLayout();
    mLayoutEventManager->setAlignment(Qt::AlignTop);
    mLayoutEventManager->setDirection(QBoxLayout::TopToBottom);
    mLayoutEventManager->setSizeConstraint(QLayout::SetMinAndMaxSize);
    ui->gb_EventTypeList->setLayout(mLayoutEventManager);


    mLayoutEventDetails=new QVBoxLayout();
    mLayoutEventDetails->setAlignment(Qt::AlignTop);
    mLayoutEventDetails->setDirection(QBoxLayout::TopToBottom);
    mLayoutEventDetails->addWidget(mEventDetails);
    ui->gb_Details->setLayout(mLayoutEventDetails);

    mLayoutDepouillement=new QVBoxLayout();
    mLayoutDepouillement->setAlignment(Qt::AlignTop);
    mLayoutDepouillement->setDirection(QBoxLayout::TopToBottom);
    mLayoutDepouillement->addWidget(mDepouillement);
    ui->gb_Depouillement->setLayout(mLayoutDepouillement);



    QObject::connect(ui->actionPreferences,&QAction::triggered,mFenPref,&FenPreferences::initFen);
    QObject::connect(ui->action_OpenConf,&QAction::triggered,mFenPref,&FenPreferences::openRepConf);
    QObject::connect(ui->action_NewLogbook,&QAction::triggered,this,&MainWindow::clickOnNewLogbook);
    QObject::connect(ui->actionOpenLogbook,&QAction::triggered,this,&MainWindow::clickOnOpenLogbook);
    QObject::connect(ui->actionsaveLogbook,&QAction::triggered,this,&MainWindow::clickOnSaveLogbook);
    QObject::connect(ui->actionDetailsMission,&QAction::triggered,this,&MainWindow::clickOnEditCruise);
    QObject::connect(ui->actionQuitter,&QAction::triggered,this,&MainWindow::close);
    QObject::connect(mFenPref,&FenPreferences::confChanged,mNavData,&NavData::initCom);
    QObject::connect(mFenPref,&FenPreferences::confChanged,this,&MainWindow::initEvents);
    QObject::connect(mFenPref,&FenPreferences::xmlError,this,&MainWindow::readError);
    QObject::connect(mNavData,&NavData::errorString,this,&MainWindow::readError);
    QObject::connect(mLogBookModel,&LogbookModel::xmlError,this,&MainWindow::readError);
    QObject::connect(mFenMission,&fenMission::newCruiseSet,this,&MainWindow::setNewLogbook);
    QObject::connect(mFenMission,&fenMission::newCruiseDetails,mLogBookModel,&LogbookModel::setCurrentCruise);
    QObject::connect(mFenMission,&fenMission::newCruiseDetails,mDepouillement,&Depouillement::setCurrentCruise);
    QObject::connect(ui->tableMain,&QTableView::clicked,mLogBookModel,&LogbookModel::selectEvent);
    QObject::connect(ui->tableMain,&QTableView::clicked,this,&MainWindow::showDetails);
    QObject::connect(mLogBookModel,&LogbookModel::eventSelected,mEventDetails,&EventDetails::initEvent);
    QObject::connect(mLogBookModel,&LogbookModel::modelIsInit,this,&MainWindow::hideColumn);
    QObject::connect(ui->btn_Details,&QToolButton::clicked,this,&MainWindow::clickOnShowDetails);
    QObject::connect(mLogBookModel,&LogbookModel::sendEventList,mTableModelProfil,&CustomTableModel::setEventList);
    QObject::connect(mLogBookModel,&LogbookModel::sendEventList,mTableModelSippican,&CustomTableModel::setEventList);
    QObject::connect(mLogBookModel,&LogbookModel::sendEventList,mTableModelSimpleEvent,&CustomTableModel::setEventList);
    QObject::connect(mLogBookModel,&LogbookModel::sendEventList,mTableModelArcCont,&CustomTableModel::setEventList);
    QObject::connect(mLogBookModel,&LogbookModel::sendEventList,mDepouillement,&Depouillement::setListEvent);
    QObject::connect(mTableModelProfil,&CustomTableModel::modelIsinit,this,&MainWindow::initCustomTable);
    QObject::connect(mTableModelSippican,&CustomTableModel::modelIsinit,this,&MainWindow::initCustomTable);
    QObject::connect(mTableModelSimpleEvent,&CustomTableModel::modelIsinit,this,&MainWindow::initCustomTable);
    QObject::connect(mTableModelArcCont,&CustomTableModel::modelIsinit,this,&MainWindow::initCustomTable);



    initEvents();
    mNavData->initCom();

    QString sMission=mFenMission->getCurrentCruiseName();
    if(sMission=="")
    {
        QMessageBox::warning(this,"Aucun cahier de quart","Aucun cahier de quart n'est configuré,\n"
                                                          "Veuillez créer une campagne");
        mFenMission->newCruise();
    }
    QString sLogbookName=QString("Logbook_%1.xml").arg(sMission);
    setCurrentLogbook(mFenPref->getCurrentLogbookDir(),sLogbookName);
    ui->gb_Details->setVisible(false);
    mDepouillement->show();

}

void MainWindow::readError(QString sError)
{
    ui->statusbar->showMessage(sError,3000);
    qDebug()<<sError;
}

void MainWindow::initEvents()
{
    if(mButtonList.size()>0)
    {
            QLayoutItem* item;
            while ( ( item = mLayoutEventManager->takeAt( 0 ) ) != NULL )
            {
                delete item->widget();
                delete item;
            }
        delete mLayoutButtonsManager;
        mLayoutButtonsManager=new QVBoxLayout();
        mLayoutButtonsManager->setAlignment(Qt::AlignTop);
        mLayoutButtonsManager->setDirection(QBoxLayout::TopToBottom);
         ui->gb_ButtonList->setLayout(mLayoutButtonsManager);
    }
    mManagerList.clear();
    mButtonList.clear();
    mListEventsType=mFenPref->getEventTypeList();
    mDepouillement->setEventSettingsList(mListEventsType);

    QListIterator<EventSettings::eventProperties>it(mListEventsType);
    while (it.hasNext()) {
        EventManager *manager=new EventManager;

        mManagerList.append(manager);
        mLayoutEventManager->addWidget(manager);
        manager->initManager(it.next());
        QPushButton *btn_Manager=new QPushButton;
        btn_Manager->setCheckable(true);
        btn_Manager->setMinimumWidth(90);
        btn_Manager->setText(manager->getName());
        QObject::connect(btn_Manager,&QPushButton::clicked,manager,&EventManager::buttonIsChecked);
        QObject::connect(mNavData,&NavData::gpsReceived,manager,&EventManager::majCurrentGPS);
        QObject::connect(mNavData,&NavData::sbeReceived,manager,&EventManager::majCurrentSBE);
        QObject::connect(mNavData,&NavData::sondeReceived,manager,&EventManager::majCurrentDepth);
        QObject::connect(mNavData,&NavData::celReceived,manager,&EventManager::majCurrentCelerimeter);
        QObject::connect(manager,&EventManager::setButtonChecked,btn_Manager,&QPushButton::setChecked);
        QObject::connect(manager,&EventManager::addEvent,mLogBookModel,&LogbookModel::addEvent);
        QObject::connect(mEventDetails,&EventDetails::modifyEvent,mLogBookModel,&LogbookModel::modifyEvent);
        QObject::connect(mEventDetails,&EventDetails::removeEvent,mLogBookModel,&LogbookModel::removeEvent);
        QObject::connect(mLogBookModel,&LogbookModel::lastEventRead,manager,&EventManager::majLastEvent);

        mButtonList.append(btn_Manager);
        mLayoutButtonsManager->addWidget(btn_Manager);
    }

}

void MainWindow::setNewLogbook(QString sMission)
{
    QString sLogbookName=QString("Logbook_%1.xml").arg(sMission);
    QString sDir=mFenPref->getDefaultLogbookDir();
    mFenPref->setCurrentLogbookDir(sDir);
    mLogBookModel->setNewLogbookPath(QString("%1/%2").arg(sDir,sLogbookName));
    mLogBookModel->setCurrentCruise(mFenMission->getCurrentCruise());
    initLogBook(QString("%1/%2").arg(sDir,sLogbookName));


}

void MainWindow::setCurrentLogbook(QString sDir,QString sLogbookName)
{

    QString sLogbookPath=QString("%1/%2").arg(sDir,sLogbookName);
    initLogBook(sLogbookPath);

}

void MainWindow::initLogBook(QString sPath)
{
    //QString sMission=sPath.section("_",1,1).section(".",0,0);

    mLogBookModel->initXML(sPath);
    mFenMission->setCurrentCruise(mLogBookModel->getCurrentCruise());
    mDepouillement->setCurrentCruise(mFenMission->getCurrentCruise());
    ui->l_CurrentLogbook->setText(QString("Mission %1").arg(mFenMission->getCurrentCruiseName()));
    ui->tableMain->setModel(mLogBookModel);
    ui->tableMain->setColumnHidden(0,true);



}

void MainWindow::initCustomTable()
{
    //mTableModelProfil->clearRows();
    ui->table_Profils->setModel(mTableModelProfil);
    ui->table_Profils->setColumnHidden(0,true);
    int i=0;
    for(i=10;i<19;i++)
        ui->table_Profils->setColumnHidden(i,true);

    ui->table_Profils->setColumnWidth(1,130);
    ui->table_Profils->setColumnWidth(2,100);
    ui->table_Profils->setColumnWidth(4,180);
    ui->table_Profils->setColumnWidth(5,80);
    ui->table_Profils->setColumnWidth(6,80);
    ui->table_Profils->setColumnWidth(7,80);
    ui->table_Profils->setColumnWidth(8,80);

    ui->table_Profils->scrollToBottom();

   // mTableModelSippican->clearRows();
    ui->table_Sippican->setModel(mTableModelSippican);
    ui->table_Sippican->setColumnHidden(0,true);
    ui->table_Sippican->setColumnHidden(8,true);
    ui->table_Sippican->setColumnHidden(9,true);
    ui->table_Sippican->scrollToBottom();

    ui->table_Sippican->setColumnWidth(1,130);
    ui->table_Sippican->setColumnWidth(2,100);
    ui->table_Sippican->setColumnWidth(4,180);
    ui->table_Sippican->setColumnWidth(5,80);
    ui->table_Sippican->setColumnWidth(6,80);
    ui->table_Sippican->setColumnWidth(7,80);
    ui->table_Sippican->setColumnWidth(10,100);
    ui->table_Sippican->setColumnWidth(11,100);
    ui->table_Sippican->setColumnWidth(12,100);
    ui->table_Sippican->setColumnWidth(13,100);
    ui->table_Sippican->setColumnWidth(14,100);
    ui->table_Sippican->setColumnWidth(15,100);
    ui->table_Sippican->setColumnWidth(16,100);
    ui->table_Sippican->setColumnWidth(17,100);

    //mTableModelSimpleEvent->clearRows();
    ui->table_EvntPonct->setModel(mTableModelSimpleEvent);
    ui->table_EvntPonct->setColumnHidden(0,true);
    for(i=8;i<19;i++)
            ui->table_EvntPonct->setColumnHidden(i,true);
    ui->table_EvntPonct->setColumnWidth(1,130);
    ui->table_EvntPonct->setColumnWidth(2,100);
    ui->table_EvntPonct->setColumnWidth(4,180);
    ui->table_EvntPonct->setColumnWidth(5,80);
    ui->table_EvntPonct->setColumnWidth(6,80);
    ui->table_EvntPonct->setColumnWidth(7,80);
    ui->table_EvntPonct->scrollToBottom();

   // mTableModelArcCont->clearRows();
    ui->table_ArcCont->setModel(mTableModelArcCont);
    ui->table_ArcCont->setColumnHidden(0,true);
    for(i=8;i<19;i++)
            ui->table_ArcCont->setColumnHidden(i,true);
    ui->table_ArcCont->setColumnWidth(1,130);
    ui->table_ArcCont->setColumnWidth(2,120);
    ui->table_ArcCont->setColumnWidth(4,180);
    ui->table_ArcCont->setColumnWidth(5,80);
    ui->table_ArcCont->setColumnWidth(6,80);
    ui->table_ArcCont->setColumnWidth(7,80);
    ui->table_ArcCont->scrollToBottom();
}

void MainWindow::hideColumn()
{
    ui->tableMain->setColumnHidden(0,true);
    ui->tableMain->setColumnWidth(1,130);
    ui->tableMain->scrollToBottom();

}

void MainWindow::clickOnShowDetails()
{
    ui->gb_Details->setVisible(!ui->gb_Details->isVisible());
    if(ui->gb_Details->isVisible())
        ui->btn_Details->setText(">>");
    else
        ui->btn_Details->setText("<<");
}

void MainWindow::showDetails()
{
     ui->gb_Details->setVisible(true);
     ui->btn_Details->setText(">>");
}

void MainWindow::clickOnNewLogbook()
{
    //mFenMission->setCurrentCruise(mLogBookModel->getCurrentCruise());
    mFenMission->newCruise();
}

void MainWindow::clickOnOpenLogbook()
{
    QString sPath=QFileDialog::getOpenFileName(this,"Sélectionner un cahier de quart",mFenPref->getCurrentLogbookDir(),"*.xml");
    if(!sPath.isEmpty())
    {
        QString sDirPath;
        int n=sPath.count("/");
        sDirPath=sPath.section("/",0,n-1);
        mFenPref->setCurrentLogbookDir(sDirPath);
        initLogBook(sPath);
    }
}

void MainWindow::clickOnSaveLogbook()
{
    QString sPath=QFileDialog::getSaveFileName(this,"Enregistrer le cahier de quart sous",mFenPref->getDefaultLogbookDir(),"*.xml");
    if(!sPath.isEmpty())
    {
        QFile::copy(mLogBookModel->getCurrentLogbookPath(),sPath);
        initLogBook(sPath);
    }
}

void MainWindow::clickOnEditCruise()
{
    mFenMission->setCurrentCruise(mLogBookModel->getCurrentCruise());
    mFenMission->editCruise();
}

MainWindow::~MainWindow()
{
    delete ui;
}

