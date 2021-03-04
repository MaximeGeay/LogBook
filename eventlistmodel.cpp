#include "eventlistmodel.h"

EventListModel::EventListModel(QObject *parent)
    : QStandardItemModel(parent)
{
    this->setColumnCount(2);

}

void EventListModel::appendEvent(EventSettings::eventProperties properties)
{
    bool bExist=false;
    if(eventNameExist(properties.name))
        bExist=true;
    else
    {
        appendData(properties);
        addElement(properties);
    }

    emit eventAlreadyExist(bExist);

}

bool EventListModel::initXML(QString sPath)
{

    if(!QFile::exists(sPath))
        creeXML(sPath);

    mXMLFileIsInit=readXML(sPath);
    if(mXMLFileIsInit)
    {
        mPathToXML=sPath;
        initModel();
    }

    return mXMLFileIsInit;

}

void EventListModel::initModel()
{
    EventSettings::eventProperties unEvent;
    mDatas.clear();
    this->clear();
    if(!mXMLFileIsInit)
        return;

    QDomElement root=mDomEventList->documentElement();
    QDomNodeList nodeList=root.childNodes();
    int nSize =nodeList.count();
    for (int n=0;n<nSize;n++)
    {
        QDomElement Component=nodeList.item(n).toElement();
        if(Component.tagName()=="EventItem")
        {
            unEvent=setAttributesToEvent(Component);
            appendData(unEvent);
        }

    }
}

void EventListModel::addElement(EventSettings::eventProperties properties)
{
    if(!mXMLFileIsInit)
    {

        return;
    }

    QDomElement docElem = mDomEventList->documentElement();

    docElem.appendChild(setEventToAttributes(properties));

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

void EventListModel::removeElement(EventSettings::eventProperties properties)
{
    QListIterator<EventSettings::eventProperties>it(mDatas);
    EventSettings::eventProperties uneData;
    bool bFound=false;
    int i=0;
    while(it.hasNext())
    {
        uneData=it.next();
        if(uneData.name.compare(properties.name,Qt::CaseInsensitive)==0)
        {
            mDatas.removeAt(i);
            bFound=true;
            break;
        }
        i++;
    }

    if(bFound)
    {
        majXML();
        initModel();

    }
}

void EventListModel::modifyElement(EventSettings::eventProperties previous, EventSettings::eventProperties newProperties)
{
    QListIterator<EventSettings::eventProperties>it(mDatas);
    EventSettings::eventProperties uneData;
    bool bFound=false;
    int i=0;
    while(it.hasNext())
    {
        uneData=it.next();
        if(uneData.name.compare(previous.name,Qt::CaseInsensitive)==0)
        {
            mDatas.replace(i,newProperties);
            bFound=true;
            break;
        }
        i++;
    }

    if(bFound)
    {
        majXML();
        initModel();
        emit eventAlreadyExist(false);
    }
}



bool EventListModel::eventNameExist(QString sName)
{
    EventSettings::eventProperties unEvent;
    bool bFound=false;

    if(!mXMLFileIsInit)
        return false;

    QDomElement root=mDomEventList->documentElement();
    QDomNodeList nodeList=root.childNodes();
    int nSize =nodeList.count();
    for (int n=0;n<nSize;n++)
    {
        QDomElement Component=nodeList.item(n).toElement();

        if(Component.tagName()=="EventItem")
        {
            unEvent=setAttributesToEvent(Component);
            if(unEvent.name.compare(sName,Qt::CaseInsensitive)==0)
            {
                bFound=true;
                break;
            }
        }

    }

    return bFound;
}

QList<EventSettings::eventProperties> EventListModel::getEventList()
{
    return mDatas;
}



EventSettings::eventProperties EventListModel::findEvent(QString sName)
{
    EventSettings::eventProperties unEvent,foundEvent;

    if(!mXMLFileIsInit)
        return unEvent;

    QDomElement root=mDomEventList->documentElement();
    QDomNodeList nodeList=root.childNodes();
    int nSize =nodeList.count();
    for (int n=0;n<nSize;n++)
    {
        QDomElement Component=nodeList.item(n).toElement();

        if(Component.tagName()=="EventItem")
        {
            unEvent=setAttributesToEvent(Component);
            if(unEvent.name.compare(sName,Qt::CaseInsensitive)==0)
            {
                foundEvent=unEvent;
                break;
            }
        }

    }

    return foundEvent;
}

void EventListModel::appendData(EventSettings::eventProperties properties)
{
    QStandardItem* iName=new QStandardItem(properties.name);
    QStandardItem* iType=new QStandardItem(properties.typeName);
    QList<QStandardItem*> list;
    list<<iName<<iType;

    mDatas.append(properties);
    this->appendRow(list);

}

EventSettings::eventProperties EventListModel::setAttributesToEvent(QDomElement Component)
{
    EventSettings::eventProperties evt;
    evt.name=Component.attribute("Name","");
    evt.typeId=Component.attribute("TypeId","0").toInt();
    evt.typeName=Component.attribute("TypeName","");

    return evt;
}

QDomElement EventListModel::setEventToAttributes(EventSettings::eventProperties properties)
{
    QDomElement xmlElement=mDomEventList->createElement("EventItem");

    xmlElement.setAttribute("Name",properties.name);
    xmlElement.setAttribute("TypeId",static_cast<int>(properties.typeId));
    xmlElement.setAttribute("TypeName",properties.typeName);

    return xmlElement;
}

bool EventListModel::majXML()
{
    bool bMajOK=false;
    creeXML(mPathToXML);
    if(readXML(mPathToXML))
    {

       QListIterator<EventSettings::eventProperties>it(mDatas);
       EventSettings::eventProperties uneData;

       while(it.hasNext())
       {
           uneData=it.next();
           qDebug()<<uneData.name;
           addElement(uneData);
       }
       bMajOK=true;
    }

    initXML(mPathToXML);
    return bMajOK;

}

bool EventListModel::creeXML(QString sPath)
{

    QFile xmlDoc(sPath);
    if(!xmlDoc.open(QIODevice::WriteOnly))
    {
        emit xmlError("Le document XML n'a pas pu être créé.");
        return false;
    }
    QXmlStreamWriter writer(&xmlDoc);
    QString uri="http://cqt.logbook.fr/EventItems";
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(uri,tr("EventItems"));
    writer.writeEndElement();
    writer.writeEndDocument();

    xmlDoc.close();

    return true;
}

bool EventListModel::readXML(QString sPath)
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


