#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <QWidget>
#include <QtXml>
#include "eventsettings.h"
#include "navdata.h"
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
    void addEvent(QDomElement xmlElement);
public slots:
    void initManager(EventSettings::eventProperties prop);
    QString getName();
    void buttonIsChecked(bool bStatus);
    void majCurrentData(NavData::Datas datas);

private slots:
    void clickOnDebProfil();
    void clickOnFinProfil();
    void clickOnAnnuler();
    void clickOnValider();
    void writeDatas();
private:
    Ui::EventManager *ui;


    EventSettings::eventProperties mEventType;
    NavData::Datas mCurrentData;
    bool mProfilStatus;

};

#endif // EVENTMANAGER_H