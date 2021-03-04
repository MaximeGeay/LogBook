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

private:
    Ui::MainWindow *ui;
    FenPreferences* mFenPref;
    NavData* mNavData;
    QHBoxLayout *mLayoutDataManager;
    QVBoxLayout *mLayoutEventManager;
    QVBoxLayout *mLayoutButtonsManager;
    QList<EventSettings::eventProperties>mListEventsType;
    QList<EventManager*>mManagerList;
    QList<QPushButton*>mButtonList;
    QDomDocument *mLogbook;
    QString mPathToXML;


};
#endif // MAINWINDOW_H
