#include "logbookmodel.h"

LogbookModel::LogbookModel(QObject *parent) : QStandardItemModel(parent)
{

}


bool LogbookModel::initXML(QString sPath)
{
    if(!QFile::exists(sPath))
        if(!creeXML(sPath))
            emit xmlError("Création du fichier de configuration XML impossible");

    mXMLFileIsInit=readXML(sPath);
    if(mXMLFileIsInit)
    {
        mPathToXML=sPath;
        initModel();
    }
    else
    {
        emit xmlError("Lecture du fichier de configuration XML impossible");
    }

    return mXMLFileIsInit;
}

void LogbookModel::addEvent(LogbookModel::stLogbookData event)
{
    event.index=mLastIndex+1;
    addDomElement(setLogbookDataToAttributes(event));
    initModel();
}

void LogbookModel::modifyEvent(LogbookModel::stLogbookData eventToFind)
{
    QListIterator<stLogbookData>it(mEventList);
    stLogbookData unEvent;
    bool bFound=false;
    int i=0;

    while(it.hasNext())
    {
        unEvent=it.next();
        if(unEvent.index==eventToFind.index)
        {
            mEventList.replace(i,eventToFind);
            bFound=true;
            break;
        }
        i++;
    }


    if(bFound)
    {
        majXML();


    }
}

void LogbookModel::removeEvent(LogbookModel::stLogbookData eventToFind)
{

    QListIterator<stLogbookData>it(mEventList);
    stLogbookData unEvent;
    bool bFound=false;
    int i=0;

    while(it.hasNext())
    {
        unEvent=it.next();
        if(unEvent.index==eventToFind.index)
        {
            mEventList.removeAt(i);
            bFound=true;
            break;
        }
        i++;
    }


    if(bFound)
    {
        majXML();


    }

}
void LogbookModel::selectEvent(const QModelIndex ind)
{
    LogbookModel::stLogbookData unEvent=findEvent(this->item(ind.row(),0)->text().toInt());

    emit eventSelected(unEvent);

}

LogbookModel::stLogbookData LogbookModel::findEvent(int nIndex)
{
    stLogbookData unEvent;
    QListIterator<stLogbookData>it(mEventList);

    while (it.hasNext())
    {
      unEvent=it.next();
      if(unEvent.index==nIndex)
      {

          return unEvent;
      }
    }

    stLogbookData error;
    error.titre="Event not found";
    error.type="Error";
    return error;
}

void LogbookModel::setCurrentCruise(fenMission::st_Mission stCruise)
{
    mCurrentCruise=stCruise;
    qDebug()<<"CurrentCruise"<<mCurrentCruise.sNom<<mCurrentCruise.sChefMission;
    majXML();
}

fenMission::st_Mission LogbookModel::getCurrentCruise()
{
    return mCurrentCruise;
}

QString LogbookModel::getCurrentLogbookPath()
{
    return mPathToXML;
}

void LogbookModel::addDomElement(QDomElement element)
{
    if(!mXMLFileIsInit)
    {
        qDebug()<<"LogbookXML non initialisé";
        return;
    }

    QDomElement docElem = mDomEventList->documentElement();

    docElem.appendChild(element);

    QString write_doc=mDomEventList->toString();

    QFile fichier(mPathToXML);
    if(!fichier.open(QIODevice::WriteOnly))
    {
        fichier.close();
        emit xmlError( "Impossible d'écrire dans le document XML");
        return;
    }

    QTextStream stream(&fichier);

    stream<<write_doc;

    fichier.close();
}

bool LogbookModel::majXML()
{
    bool bMajOK=false;

    creeXML(mPathToXML);

    if(readXML(mPathToXML))
    {

       addDomElement(setMissionDataToAttributes(mCurrentCruise));

       QListIterator<stLogbookData>it(mEventList);
        stLogbookData unEvent;
       while(it.hasNext())
       {
           unEvent=it.next();
          addDomElement(setLogbookDataToAttributes(unEvent));
       }
       bMajOK=true;
    }


    initXML(mPathToXML);
    initModel();

    return bMajOK;
}

bool LogbookModel::creeXML(QString sPath)
{
    QFile xmlDoc(sPath);
    if(!xmlDoc.open(QIODevice::WriteOnly))
    {
        emit xmlError("Le document Logbook XML n'a pas pu être créé.");
        return false;
    }
    QXmlStreamWriter writer(&xmlDoc);
    QString uri="http://cqt.logbook.fr/Events";
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(uri,tr("Events"));
    writer.writeEndElement();
    writer.writeEndDocument();

    xmlDoc.close();

    return true;
}

bool LogbookModel::readXML(QString sPath)
{
    mDomEventList=new QDomDocument;


    QFile xmlDoc(sPath);
    if(xmlDoc.exists())
    {

        if(!xmlDoc.open(QIODevice::ReadOnly))
        {
            emit xmlError("Le document XML n'a pas pu être ouvert.");

            return false;
        }
        if (!mDomEventList->setContent(&xmlDoc))
        {
            xmlDoc.close();
            emit xmlError("Le document XML n'a pas pu être attribué à l'objet QDomDocument.");
            return false;
        }

        xmlDoc.close();
        mPathToXML=sPath;

        return true;
    }

    emit xmlError("Le document XML n'existe pas.");
    return false;
}

void LogbookModel::initModel()
{
    stLogbookData uneLigne;
    mEventList.clear();
    mLastIndex=0;
    this->clear();

    QStringList labels;
    labels<<"Index"<<"Date/Heure"<<"Equipement"<<"Evènement"<<"Commentaires";
    this->setHorizontalHeaderLabels(labels);



    if(!mXMLFileIsInit)
        return;

    QDomElement root=mDomEventList->documentElement();
    QDomNodeList nodeList=root.childNodes();
    int nSize =nodeList.count();
    for (int n=0;n<nSize;n++)
    {
        QDomElement Component=nodeList.item(n).toElement();

        if(Component.tagName()=="Cruise")
        {
            mCurrentCruise=setAttributesToMissionData(Component);

        }
        if(Component.tagName()=="Event")
        {
            uneLigne=setAttributesToLogbookData(Component);
            mLastIndex+=1;

            appendData(uneLigne);

        }

    }

    emit sendEventList(mEventList);
    emit modelIsInit();
}

void LogbookModel::appendData(LogbookModel::stLogbookData unEvent)
{
    QStandardItem* iID=new QStandardItem(QString::number(unEvent.index));
    QStandardItem* iDate=new QStandardItem(unEvent.dateheure.toString(("dd/MM/yyyy hh:mm:ss")));
    QStandardItem* iTitre=new QStandardItem(unEvent.titre);
    QStandardItem* iEquipement=new QStandardItem(unEvent.equipement);
    QStandardItem* iCommentaire=new QStandardItem(unEvent.commentaire);

    QList<QStandardItem*> list;
    list<<iID<<iDate<<iEquipement<<iTitre<<iCommentaire;

    mEventList.append(unEvent);
    emit lastEventRead(unEvent);
    this->appendRow(list);

}

LogbookModel::stLogbookData LogbookModel::setAttributesToLogbookData(QDomElement element)
{
    stLogbookData data;
    data.equipement=element.attribute("Equipement","");
    data.type=element.attribute("Type","");
    data.typeId=element.attribute("TypeId","0").toInt();
    data.titre=element.attribute("Titre","");
    data.commentaire=element.attribute("Commentaires","");
    data.dateheure=QDateTime::fromString(element.attribute("DateHeure","01/01/2000 00:00:00"),"dd/MM/yyyy hh:mm:ss");
    data.position=stringToPos(element.attribute("Position",QString("0° 0.0' N, 0° 0.0' E")));
    data.cog=element.attribute("COG","0").toDouble();
    data.sog=element.attribute("SOG","0").toDouble();
    data.sonde=element.attribute("Sonde","0").toDouble();
    data.ficProfil=element.attribute("FichierProfil","");
    data.ficASVP=element.attribute("FichierASVP","");
    data.profil=element.attribute("Profil","0").toInt();
    data.probeNumber=element.attribute("Probe","0").toInt();
    data.typeSippican=element.attribute("ProbeType","");
    data.tempSBE=element.attribute("TempSBE","0").toDouble();
    data.salinite=element.attribute("Salinite","0").toDouble();
    data.celSBE=element.attribute("CelSBE","0").toDouble();
    data.refCelerite=element.attribute("RefCelerite","0").toInt();
    data.celerimetre=element.attribute("Celerimetre","0").toDouble();
    data.celSippican=element.attribute("CelSippican","0").toDouble();
    data.sisLoaded=element.attribute("ChargeSIS","0").toInt();
    data.dateheureSIS=QDateTime::fromString(element.attribute("DateHeureChargeSIS","01/01/2000 00:00:00"),"dd/MM/yyyy hh:mm:ss");
    data.ficSIS=element.attribute("FichierSIS","");
    data.index=element.attribute("Index","0").toInt();

    return data;
}

QDomElement LogbookModel::setLogbookDataToAttributes(LogbookModel::stLogbookData data)
{
    QDomDocument dom;
    QDomElement element=dom.createElement("Event");
    element.setAttribute("Equipement",data.equipement);
    element.setAttribute("Type",data.type);
    element.setAttribute("TypeId",QString::number(data.typeId));
    element.setAttribute("Titre",data.titre);
    element.setAttribute("Commentaires",data.commentaire);
    element.setAttribute("DateHeure",data.dateheure.toString("dd/MM/yyyy hh:mm:ss"));
    element.setAttribute("Position",data.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere));
    element.setAttribute("COG",QString::number(data.cog,'f',1));
    element.setAttribute("SOG",QString::number(data.sog,'f',1));
    element.setAttribute("Sonde",QString::number(data.sonde,'f',1));
    element.setAttribute("FichierProfil",data.ficProfil);
     element.setAttribute("FichierASVP",data.ficASVP);
    element.setAttribute("Profil",QString::number(data.profil));
    element.setAttribute("Probe",QString::number(data.probeNumber));
    element.setAttribute("ProbeType",data.typeSippican);
    element.setAttribute("TempSBE",QString::number(data.tempSBE,'f',1));
    element.setAttribute("Salinite",QString::number(data.salinite,'f',1));
    element.setAttribute("CelSBE",QString::number(data.celSBE,'f',1));
    element.setAttribute("RefCelerite",QString::number(data.refCelerite));
    element.setAttribute("Celerimetre",QString::number(data.celerimetre,'f',1));
    element.setAttribute("CelSippican",QString::number(data.celSippican,'f',1));
    element.setAttribute("ChargeSIS",QString::number(data.sisLoaded));
    element.setAttribute("DateHeureChargeSIS",data.dateheureSIS.toString("dd/MM/yyyy hh:mm:ss"));
    element.setAttribute("FichierSIS",data.ficSIS);
    element.setAttribute("Index",QString::number(data.index));
    return element;

}

fenMission::st_Mission LogbookModel::setAttributesToMissionData(QDomElement element)
{
    fenMission::st_Mission cruise;
    cruise.sNom=element.attribute("Nom","");
    cruise.dateDebut=QDate::fromString(element.attribute("DateDebut","01/01/2000"),"dd/MM/yyyy");
    cruise.dateFin=QDate::fromString(element.attribute("DateFin","01/01/2000"),"dd/MM/yyyy");
    cruise.sNavire=element.attribute("Navire","");
    cruise.sZone=element.attribute("Zone","");
    cruise.sChefMission=element.attribute("ChefMission","");
    cruise.sOperateurs=element.attribute("Operateurs","");
    return cruise;
}

QDomElement LogbookModel::setMissionDataToAttributes(fenMission::st_Mission data)
{
    QDomDocument dom;
    QDomElement element=dom.createElement("Cruise");
    element.setAttribute("Nom",data.sNom);
    element.setAttribute("DateDebut",data.dateDebut.toString("dd/MM/yyyy"));
    element.setAttribute("DateFin",data.dateFin.toString("dd/MM/yyyy"));
    element.setAttribute("Navire",data.sNavire);
    element.setAttribute("Zone",data.sZone);
    element.setAttribute("ChefMission",data.sChefMission);
    element.setAttribute("Operateurs",data.sOperateurs);
    return element;
}

QGeoCoordinate LogbookModel::stringToPos(QString sPos)
{

    //47° 37.264' N, 1° 18.254' E
    QString sLat=sPos.section(",",0,0);
    QString sLong=sPos.section(",",1,1);
    QGeoCoordinate geo(latMinToDec(sLat),longMinToDec(sLong));
    return geo;
}

double LogbookModel::latMinToDec(QString sLatitude)
{
    //format : DD° MM.ddd' H
    //         47° 37.264' N
        int nDeg=sLatitude.section("°",0,0).toInt();
        double dMin=sLatitude.section("°",1,1).section("'",0,0).remove(" ").toDouble();
        QString cHemi=sLatitude.section("'",1,1).remove(" ");

        double dLatDec=nDeg+(dMin/60);
        if(cHemi=="S")
            dLatDec=-dLatDec;

        return dLatDec;
}

double LogbookModel::longMinToDec(QString sLongitude)
{
    //format :DD° MM.ddd' H
    //        1° 18.254' E
    int nDeg=sLongitude.section("°",0,0).toInt();
    double dMin=sLongitude.section("°",1,1).section("'",0,0).remove(" ").toDouble();
    QString cHemi=sLongitude.section("'",1,1).remove(" ");

    double dLongDec=nDeg+(dMin/60);
    if(cHemi=="W")
        dLongDec=-dLongDec;

    return dLongDec;
}
