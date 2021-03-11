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
    void creeLogbook(QString sMission);

private:
    Ui::MainWindow *ui;
    FenPreferences* mFenPref;
    NavData* mNavData;
    fenMission* mFenMission;
    LogbookModel* mLogBookModel;
    QHBoxLayout *mLayoutDataManager;
    QVBoxLayout *mLayoutEventManager;
    QVBoxLayout *mLayoutButtonsManager;
    QList<EventSettings::eventProperties>mListEventsType;
    QList<EventManager*>mManagerList;
    QList<QPushButton*>mButtonList;

    QString mPathToXML;


};
#endif // MAINWINDOW_H
