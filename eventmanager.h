#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <QWidget>
#include <QtXml>
#include <QBoxLayout>
#include "eventsettings.h"
#include "navdata.h"
#include "logbookmodel.h"
#include "eventdetails.h"

namespace Ui {
class EventManager;
}

class EventManager : public QWidget
{
    Q_OBJECT

public:
    explicit EventManager(QWidget *parent = nullptr);
    ~EventManager();

signals:
    void setButtonChecked(bool);
    void addEvent(LogbookModel::stLogbookData);
public slots:
    void initManager(EventSettings::eventProperties prop);
    QString getName();
    void buttonIsChecked(bool bStatus);
    void majCurrentGPS(NavData::stGPSDatas dataGPS);
    void majCurrentSBE(NavData::stSBEDatas dataSBE);
    void majCurrentDepth(double dSonde);
    void majCurrentCelerimeter(double dCelerimetre);
    void majLastEvent(LogbookModel::stLogbookData lastEvent);

private slots:
    void clickOnDebProfil();
    void clickOnFinProfil();
    void clickOnSippican();
    void clickOnNewEvent();
    void clickOnMajDatas();
    void clickOnDebAcquisition();
    void clickOnFinAcquisition();
    void eventAdded(LogbookModel::stLogbookData stEvent);
    void addEventFinished(bool bValid);
private:
    Ui::EventManager *ui;


    void majDetails();
    void majStatus();
    EventSettings::eventProperties mEventType;

    NavData::stSBEDatas mSBERetenue;
    NavData::stSensorsDatas mCurrentSensors;
    NavData::stSensorsDatas mSensorsRetenus;
    bool mProfilStatus=false;
    bool mAcquisitionStatus=false;
    bool mEdition=false;
    QVBoxLayout *mLayoutEventDetails;
    EventDetails* mEventDetails;
    LogbookModel::stLogbookData mCurrentLog;


};

#endif // EVENTMANAGER_H
