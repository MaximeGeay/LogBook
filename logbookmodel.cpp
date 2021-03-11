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
        //initModel();
    }
    else
    {
        emit xmlError("Lecture du fichier de configuration XML impossible");
    }

    return mXMLFileIsInit;
}

void LogbookModel::addElement(QDomElement element)
{
    if(!mXMLFileIsInit)
    {
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
