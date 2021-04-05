#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPushButton>
#include <QtXml>
#include "fenpreferences.h"
#include "navdata.h"
#include "eventsettings.h"
#include "eventmanager.h"
#include "fenmission.h"
#include "logbookmodel.h"
#include "eventdetails.h"
#include "customtablemodel.h"
#include "depouillement.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readError(QString sError);
    void initEvents();
    void setNewLogbookName(QString sMission);
    void setCurrentLogbook(QString sLogbookName);
    void initLogBook(QString sPath);
    void initCustomTable();
    void hideColumn();
    void clickOnShowDetails();
    void showDetails();
    void clickOnNewLogbook();
    void clickOnOpenLogbook();
    void clickOnSaveLogbook();
    void clickOnEditCruise();


private:
    Ui::MainWindow *ui;
    FenPreferences* mFenPref;
    NavData* mNavData;
    fenMission* mFenMission;
    LogbookModel* mLogBookModel;
    EventDetails* mEventDetails;
    Depouillement* mDepouillement;
    QHBoxLayout *mLayoutDataManager;
    QVBoxLayout *mLayoutEventManager;
    QVBoxLayout *mLayoutButtonsManager;
    QVBoxLayout *mLayoutEventDetails;
    QVBoxLayout *mLayoutDepouillement;
    QList<EventSettings::eventProperties>mListEventsType;
    QList<EventManager*>mManagerList;
    QList<QPushButton*>mButtonList;

    CustomTableModel* mTableModelProfil;
    CustomTableModel* mTableModelSippican;
    CustomTableModel* mTableModelSimpleEvent;
    CustomTableModel* mTableModelArcCont;

    //QString mPathToXML;
    //QString mLogbookName;
    //QString mCurrentCruise;


};


#endif // MAINWINDOW_H
