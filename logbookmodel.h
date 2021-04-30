#ifndef LOGBOOKMODEL_H
#define LOGBOOKMODEL_H

#include <QStandardItemModel>
#include <QtXml>
#include <QGeoCoordinate>

#include "fenmission.h"
class LogbookModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum TypeEvent { Profil=1, Sippican=2,Ponctuel=3,ArcContinu=4};
    struct stLogbookData{
        QString equipement="";
        int typeId=0;
        QString type="";
        QDateTime dateheure;
        QGeoCoordinate position;
        double cog=0;
        double sog=0;
        QString titre="";
        QString ficProfil="";
        QString ficASVP="";
        int profil=0;
        QString commentaire="";
        int probeNumber=0;
        QString typeSippican="";
        double tempSBE=0;
        double salinite=0;
        double celSBE=0;
        double celerimetre=0;
        double celSippican=0;
        double sonde=0;
        bool refCelerite=false;
        bool sisLoaded=false;
        QDateTime dateheureSIS;
        QString ficSIS="";
        int index=0;


    };

    explicit LogbookModel(QObject *parent = nullptr);




signals:
    void xmlError(QString sMsg);
    void eventSelected(stLogbookData);
    void modelIsInit();
    void lastEventRead(stLogbookData);
    void sendEventList(QList<stLogbookData>);

public slots:
    bool initXML(QString sPath);
    void addEvent(stLogbookData event);
    void modifyEvent(stLogbookData eventToFind);
    void removeEvent(stLogbookData eventToFind);

    void selectEvent(const QModelIndex ind);
    stLogbookData findEvent(int nIndex);
    void setCurrentCruise(fenMission::st_Mission stCruise);
    fenMission::st_Mission getCurrentCruise();
    QString getCurrentLogbookPath();
    void setNewLogbookPath(QString sPath);


private:
    QDomDocument *mDomEventList;
    bool mXMLFileIsInit=false;
    QString mPathToXML;


   // void addElement(stLogbookData uneLigne);
    void addDomElement(QDomElement element);
    void addElementList(QList<QDomElement> elementList);
    bool majXML();
    bool creeXML(QString sPath);
    bool readXML(QString sPath);
    void initModel();
    void appendData(stLogbookData unEvent);

    stLogbookData setAttributesToLogbookData(QDomElement element);
    QDomElement setLogbookDataToAttributes(stLogbookData data);
    fenMission::st_Mission setAttributesToMissionData(QDomElement element);
    QDomElement setMissionDataToAttributes(fenMission::st_Mission data);
    QGeoCoordinate stringToPos(QString sPos);
    double latMinToDec(QString sLatitude);
    double longMinToDec(QString sLongitude);

    int mLastIndex=0;
    QList<stLogbookData> mEventList;
    fenMission::st_Mission mCurrentCruise;

};

#endif // LOGBOOKMODEL_H
