#include "customtablemodel.h"

CustomTableModel::CustomTableModel()
{
    ;
}

CustomTableModel::~CustomTableModel()
{

}

CustomTableModel::CustomTableModel(int nType)
{
    setTypeId(nType);
    initModel();
}

void CustomTableModel::setTypeId(int nType)
{
    mTypeId=nType;

}

void CustomTableModel::setEventList(QList<LogbookModel::stLogbookData> eventList)
{
    initModel();

    LogbookModel::stLogbookData unEvent;

    QListIterator<LogbookModel::stLogbookData>it(eventList);
    while(it.hasNext())
    {
        unEvent=it.next();

        if(unEvent.typeId==mTypeId)
        {

            QList<QStandardItem*> list;
            QStandardItem* iID=new QStandardItem(QString::number(unEvent.index));
            QStandardItem* iDate=new QStandardItem(unEvent.dateheure.toString(("dd/MM/yyyy hh:mm:ss")));
            QStandardItem* iEquipement=new QStandardItem(unEvent.equipement);
            QStandardItem* iTitre=new QStandardItem(unEvent.titre);
            QStandardItem* iCommentaire=new QStandardItem(unEvent.commentaire);
            QStandardItem* iPosition=new QStandardItem(unEvent.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere));
            QStandardItem* iCOG=new QStandardItem(QString::number(unEvent.cog,'f',1));
            QStandardItem* iSOG=new QStandardItem(QString::number(unEvent.sog,'f',1));
            QStandardItem* iSonde=new QStandardItem(QString::number(unEvent.sonde,'f',1));
            QStandardItem* iNProfil=new QStandardItem(QString::number(unEvent.profil));
            QStandardItem* iFichier=new QStandardItem(unEvent.ficProfil);
            QStandardItem* iCelASVP=new QStandardItem(QString::number(unEvent.celSippican,'f',1));
            QStandardItem* iCelerimetre=new QStandardItem(QString::number(unEvent.celerimetre,'f',1));
            QStandardItem* iCelSBE=new QStandardItem(QString::number(unEvent.celSBE,'f',1));
            QStandardItem* iSalinite=new QStandardItem(QString::number(unEvent.salinite,'f',1));
            QStandardItem* iTemperature=new QStandardItem(QString::number(unEvent.tempSBE,'f',1));
            QStandardItem* iProbeType=new QStandardItem(unEvent.typeSippican);
            QStandardItem* iFicASVP=new QStandardItem(unEvent.ficASVP);
            QStandardItem* iFicSIS=new QStandardItem(unEvent.ficSIS);
            QStandardItem* iDateSIS=new QStandardItem(unEvent.dateheureSIS.toString("dd/MM/yyyy hh:mm:ss"));


            list<<iID<<iDate<<iEquipement<<iTitre<<iPosition<<iCOG<<iSOG<<iSonde<<iNProfil<<iFichier<<iCelASVP<<iCelerimetre<<iCelSBE<<iSalinite<<iTemperature<<iProbeType<<iFicASVP<<iFicSIS<<iDateSIS<<iCommentaire;
            this->appendRow(list);

        }

    }





}

void CustomTableModel::clearRows()
{
    for(int i=0;i<=this->rowCount();i++)
        this->removeRow(i);
}

void CustomTableModel::initModel()
{
    this->clear();

    QStringList labels;
    labels<<"Index"<<"Date/Heure"<<"Equipement"<<"Titre"<<"Position"<<"COG"<<"SOG"<<"Sonde"<<"N°de profil"<<"Fichier"<<"Célérité à 3m" <<"Célérimètre"<<"Célérité SBE"<<"Salinité"<<"Température"<<"Type de Sonde"<<"Fichier ASVP"<<"Fichier SIS"<<"Heure de chargement"<<"Commentaires";
    this->setHorizontalHeaderLabels(labels);
    emit modelIsinit();
}
