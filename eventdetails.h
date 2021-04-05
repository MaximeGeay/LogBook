#ifndef EVENTDETAILS_H
#define EVENTDETAILS_H

#include <QWidget>
#include "logbookmodel.h"
#include "navdata.h"

namespace Ui {
class EventDetails;
}

class EventDetails : public QWidget
{
    Q_OBJECT

public:
    explicit EventDetails(QWidget *parent = nullptr);
    ~EventDetails();
    void initEvent(LogbookModel::stLogbookData event);

public slots:
    void ajouterEvent(LogbookModel::stLogbookData stEvent);
    void majSensorsData(NavData::stSensorsDatas sensors);
    void lock(bool bLock);


private slots:
    void clickOnUnlock();
    void clickOnLock();
    void clickOnModifier();
    void clickOnSupprimer();
    void clickOnValider();
    void clickOnAnnuler();
    void SISChecked(bool bCheck);


signals:
    void addEvent(LogbookModel::stLogbookData stEvent);
    void modifyEvent(LogbookModel::stLogbookData stEvent);
    void removeEvent(LogbookModel::stLogbookData stEvent);
    void getCurrentSensors();
    void editingFinished(bool);

private:
    Ui::EventDetails *ui;
    LogbookModel::stLogbookData mCurrentEvent;
    QGeoCoordinate stringToPos(QString sPos);
    double latMinToDec(QString sLatitude);
    double longMinToDec(QString sLongitude);
    NavData::stGPSDatas mGPSRetenu;
    NavData::stSBEDatas mSBERetenue;
    double mCelerimetreRetenu=0;
    double mSondeRetenue=0;
    bool mAjout=false;
    bool mValidation=false;


};

#endif // EVENTDETAILS_H
