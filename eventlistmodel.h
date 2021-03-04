#ifndef EVENTLISTMODEL_H
#define EVENTLISTMODEL_H

#include <QStandardItemModel>
#include <QtXml>
#include "eventsettings.h"
class EventListModel : public QStandardItemModel
{
    Q_OBJECT



public:
    explicit EventListModel(QObject *parent = nullptr);
    void appendEvent(EventSettings::eventProperties properties);

public slots:
    bool initXML(QString sPath);
    void initModel();
    void addElement(EventSettings::eventProperties properties);
    void removeElement(EventSettings::eventProperties properties);
    void modifyElement(EventSettings::eventProperties previous,EventSettings::eventProperties newProperties);
    bool eventNameExist(QString sName);
    QList<EventSettings::eventProperties> getEventList();

    EventSettings::eventProperties findEvent(QString sName);


signals:
    void xmlError(QString sMsg);
    void eventAlreadyExist(bool);

private:
    QDomDocument *mDomEventList;
    bool mXMLFileIsInit=false;
    QString mPathToXML;
    QList<EventSettings::eventProperties>mDatas;
    void appendData(EventSettings::eventProperties properties);
    EventSettings::eventProperties setAttributesToEvent(QDomElement Component);
    QDomElement setEventToAttributes(EventSettings::eventProperties properties);
    bool majXML();
    bool creeXML(QString sPath);
    bool readXML(QString sPath);



};

#endif // EVENTLISTMODEL_H
