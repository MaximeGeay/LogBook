#ifndef DEPOUILLEMENT_H
#define DEPOUILLEMENT_H

#include <QWidget>
#include <QGeoCoordinate>
#include "logbookmodel.h"
#include "eventsettings.h"
#include "fenmission.h"
namespace Ui {
class Depouillement;
}

class Depouillement : public QWidget
{
    Q_OBJECT

public:

    struct stProfil{

        qint64 duree=0;
        double distance=0;
        LogbookModel::stLogbookData eventDebut,eventFin;

    };
    struct stArcCont{
          qint64 duree=0;
        LogbookModel::stLogbookData eventDebut,eventFin;
    };
    struct stResumeProfil{
        qint64 dureeTotale=0;
        double distanceTotale=0;
        QString sEquipement;
        int nTotalProfils=0;
    };

    struct stResumeArcCont{
        qint64 dureeTotale=0;
        QString sEquipement;
    };

    explicit Depouillement(QWidget *parent = nullptr);
    ~Depouillement();

public slots:
    void setEventSettingsList(QList<EventSettings::eventProperties> sList);
    void setListEvent(QList<LogbookModel::stLogbookData> sList);
    void setCurrentCruise(fenMission::st_Mission sCruise);
    void process();


private slots:

     void construitTableProfil();
     void construitTableArcCont();
     void clickOnExport();



private:
    Ui::Depouillement *ui;

    void showProfils();
    QList<stProfil> findProfilsFromEvents(QString sEquipement);
    int findSippicanNumberFomEvents();
    QList<stArcCont> findArcContFromEvents(QString sEquipement);
    QPair<int,int> findBornesProfilNumber(QString sEquipement);
    QString metersToNMString(double meters);
    void analyseData();
    void exportCSV();
    void exportHTML();
    void exportGPX();

    QString secToString(qint64 nSec);
    QList<EventSettings::eventProperties> mListEventTypes;
    QList<LogbookModel::stLogbookData> mListEvent;

    QList<QList<stProfil>>mListProfils;
    QList<stResumeProfil>mListResumeProfils;
    QList<QList<stArcCont>>mListArcCont;
    QList<stResumeArcCont>mListResumeArcCont;
    int mNumSippican=0;
    fenMission::st_Mission mCurrentCruise;
    QString mRepExport;
    QSettings *mSettings;
};

#endif // DEPOUILLEMENT_H
