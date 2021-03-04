#ifndef EVENTSETTINGS_H
#define EVENTSETTINGS_H

#include <QWidget>


namespace Ui {
class EventSettings;
}

class EventSettings : public QWidget
{
    Q_OBJECT

public:


    struct eventProperties{
        QString name="";
        int typeId=0;
        QString typeName="";
    };

    explicit EventSettings(QWidget *parent = nullptr);
    ~EventSettings();


signals:
 void addEvent(eventProperties properties);
 void eventChanged(eventProperties previous,eventProperties newProperties);

public slots:
 void eventIsInvalid(bool bExist);
 void newEvent();
 void modifyEvent(eventProperties prop);

private slots:
 void clickOnValider();
 void clickOnAnnuler();

private:
    Ui::EventSettings *ui;

    bool mIsNew=false;
    eventProperties mCurrentProperties;

};

#endif // EVENTSETTINGS_H
