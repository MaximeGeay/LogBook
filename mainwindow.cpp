
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"


#define version "LogBook 0.2"
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


    QObject::connect(ui->actionPreferences,&QAction::triggered,mFenPref,&FenPreferences::initFen);
    QObject::connect(ui->action_OpenConf,&QAction::triggered,mFenPref,&FenPreferences::openRepConf);
    QObject::connect(ui->action_NewLogbook,&QAction::triggered,mFenMission,&fenMission::newCruise);
    QObject::connect(mFenPref,&FenPreferences::confChanged,mNavData,&NavData::initCom);
    QObject::connect(mFenPref,&FenPreferences::confChanged,this,&MainWindow::initEvents);
    QObject::connect(mFenPref,&FenPreferences::xmlError,this,&MainWindow::readError);
    QObject::connect(mNavData,&NavData::errorString,this,&MainWindow::readError);
    QObject::connect(mLogBookModel,&LogbookModel::xmlError,this,&MainWindow::readError);
    QObject::connect(mFenMission,&fenMission::newCruiseSet,this,&MainWindow::creeLogbook);


    initEvents();
    mNavData->initCom();
    creeLogbook(mFenMission->getCurrentCruiseName());
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


    QListIterator<EventSettings::eventProperties>it(mListEventsType);
    while (it.hasNext()) {
        EventManager *manager=new EventManager;

        mManagerList.append(manager);
        mLayoutEventManager->addWidget(manager);
        manager->initManager(it.next());
        QPushButton *btn_Manager=new QPushButton;
        btn_Manager->setCheckable(true);
        btn_Manager->setText(manager->getName());
        QObject::connect(btn_Manager,&QPushButton::clicked,manager,&EventManager::buttonIsChecked);
        QObject::connect(mNavData,&NavData::dataReceived,manager,&EventManager::majCurrentData);
        QObject::connect(manager,&EventManager::setButtonChecked,btn_Manager,&QPushButton::setChecked);
        QObject::connect(manager,&EventManager::addEvent,mLogBookModel,&LogbookModel::addElement);

        mButtonList.append(btn_Manager);
        mLayoutButtonsManager->addWidget(btn_Manager);
    }

}

void MainWindow::creeLogbook(QString sMission)
{
    if(sMission=="")
    {
        QMessageBox::warning(this,"Aucun cahier de quart","Aucun cahier de quart n'est configuré,\n"
                                                          "Veuillez créer une campagne");
        mFenMission->newCruise();
    }
    QString sDir=mFenPref->getLogbookDir();
    QString sLogbookName=QString("Logbook_%1.xml").arg(sMission);
    QString sLogbookPath=QString("%1/%2").arg(sDir,sLogbookName);
    ui->l_CurrentLogbook->setText(sLogbookPath);
    mLogBookModel->initXML(sLogbookPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

