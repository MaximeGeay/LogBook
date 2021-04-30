#include <QTableWidget>
//#include <QBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

#include "depouillement.h"
#include "ui_depouillement.h"

Depouillement::Depouillement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Depouillement)
{
    ui->setupUi(this);

    mSettings=new QSettings();
    mRepExport=mSettings->value("ExportPath",QCoreApplication::applicationDirPath()).toString();

    mLayoutProfils=new QHBoxLayout;
    mLayoutArcCont=new QHBoxLayout;

    ui->gb_Profils->setLayout(mLayoutProfils);
    ui->gb_ArcCont->setLayout(mLayoutArcCont);

    QObject::connect(ui->btn_details,&QPushButton::clicked,this,&Depouillement::construitTableProfil);
    QObject::connect(ui->btn_details,&QPushButton::clicked,this,&Depouillement::construitTableArcCont);
    QObject::connect(ui->btn_export,&QPushButton::clicked,this,&Depouillement::clickOnExport);
    QObject::connect(ui->btn_Refresh,&QPushButton::clicked,this,&Depouillement::process);
}

Depouillement::~Depouillement()
{
    delete ui;
}

void Depouillement::setEventSettingsList(QList<EventSettings::eventProperties> sList)
{
    mListEventTypes=sList;

}

void Depouillement::setListEvent(QList<LogbookModel::stLogbookData> sList)
{
    mListEvent=sList;

    process();

}

void Depouillement::setCurrentCruise(fenMission::st_Mission sCruise)
{
    mCurrentCruise=sCruise;
    ui->l_Mission->setText(QString("Mission: %1").arg(mCurrentCruise.sNom));
    ui->l_Dates->setText(QString("Du %1 au %2").arg(mCurrentCruise.dateDebut.toString("dd/MM/yyyy"),mCurrentCruise.dateFin.toString("dd/MM/yyyy")));
    ui->l_ChefMission->setText(QString("Chef(fe) de mission: %1").arg(mCurrentCruise.sChefMission));
    ui->l_Navire->setText(mCurrentCruise.sNavire);
    ui->l_Zone->setText(QString("Zone: %1").arg(mCurrentCruise.sZone));
    ui->l_Operateurs->setText(QString("Opérateurs(tices): %1").arg(mCurrentCruise.sOperateurs));


}

void Depouillement::process()
{
    mListArcCont.clear();
    mListProfils.clear();

    QListIterator<EventSettings::eventProperties>it(mListEventTypes);
    EventSettings::eventProperties unEventType;
    while(it.hasNext())
    {
        unEventType=it.next();
        if(unEventType.typeId==LogbookModel::Profil)
        {
            QList<stProfil> profilsList;

            profilsList.append(findProfilsFromEvents(unEventType.name));

            mListProfils.append(profilsList);
        }
        if(unEventType.typeId==LogbookModel::ArcContinu)
        {
            QList<stArcCont> arcContList=findArcContFromEvents(unEventType.name);
            mListArcCont.append(arcContList);
        }
    }

    analyseData();
}

void Depouillement::construitTableProfil()
{
    QListIterator<QList<stProfil>>itGlobal(mListProfils);
    QList<stProfil>uneListe;
    int n=0;
    while(itGlobal.hasNext())
    {
        uneListe=itGlobal.next();
        QTableWidget* wTable=new QTableWidget();
        wTable->setColumnCount(10);
        wTable->setRowCount(uneListe.size());

        QStringList labels;
        labels<<"Equipement"<<"N°de profil"<<"Fichier début"<<"Fichier Fin"<<"Heure Début"<<"Heure Fin"<<"Durée"<<"Distance (NM)";
        wTable->setHorizontalHeaderLabels(labels);
        wTable->setColumnCount(labels.size());
        wTable->setRowCount(uneListe.size());
        wTable->setColumnWidth(4,150);
        wTable->setColumnWidth(5,150);
        wTable->setColumnWidth(6,220);
        wTable->setMinimumSize(1100,300);
        wTable->setGeometry(0,0+n*100,1100,300);
        wTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        wTable->setSelectionMode(QAbstractItemView::SingleSelection);
        wTable->setAlternatingRowColors(true);
        wTable->setSortingEnabled(true);

        bool bFirst=true;
        QListIterator<stProfil>itProfil(uneListe);
        stProfil unProfil;
        int i=0;
        while(itProfil.hasNext())
        {
            unProfil=itProfil.next();
            if(bFirst)
            {
                wTable->setWindowTitle(unProfil.eventDebut.equipement);
                bFirst=false;
            }

            wTable->setItem(i,0,new QTableWidgetItem(unProfil.eventDebut.equipement));
            wTable->setItem(i,1,new QTableWidgetItem(QString::number(unProfil.eventDebut.profil)));
            wTable->setItem(i,2,new QTableWidgetItem(unProfil.eventDebut.ficProfil));
            wTable->setItem(i,3,new QTableWidgetItem(unProfil.eventFin.ficProfil));
            wTable->setItem(i,4,new QTableWidgetItem(unProfil.eventDebut.dateheure.toString("dd/MM/yyyy hh:mm:ss")));
            wTable->setItem(i,5,new QTableWidgetItem(unProfil.eventFin.dateheure.toString("dd/MM/yyyy hh:mm:ss")));
            wTable->setItem(i,6,new QTableWidgetItem(secToString(unProfil.duree)));
            wTable->setItem(i,7,new QTableWidgetItem(metersToNMString(unProfil.distance)));



            i++;
        }

        n++;
        wTable->show();

    }


}

void Depouillement::construitTableArcCont()
{
    QListIterator<QList<stArcCont>>itGlobal(mListArcCont);
    QList<stArcCont>uneListe;
    int n=0;
    while(itGlobal.hasNext())
    {
        uneListe=itGlobal.next();
        QTableWidget* wTable=new QTableWidget();
        wTable->setColumnCount(10);
        wTable->setRowCount(uneListe.size());
        QStringList labels;
        labels<<"Equipement"<<"Heure Début"<<"Heure Fin"<<"Durée";
        wTable->setHorizontalHeaderLabels(labels);
        wTable->setColumnCount(labels.size());
        wTable->setRowCount(uneListe.size());
        wTable->setColumnWidth(1,150);
        wTable->setColumnWidth(2,150);
        wTable->setColumnWidth(3,220);
        wTable->setMinimumSize(700,300);
        wTable->setGeometry(0,0+n*100,700,300);
        wTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        wTable->setSelectionMode(QAbstractItemView::SingleSelection);
        wTable->setAlternatingRowColors(true);
        wTable->setSortingEnabled(true);

        QListIterator<stArcCont>itArc(uneListe);
        stArcCont unArchi;
        int i=0;
        bool bFirst=true;
        while(itArc.hasNext())
        {
            unArchi=itArc.next();
            if(bFirst)
            {
                wTable->setWindowTitle(unArchi.eventDebut.equipement);
                bFirst=false;
            }
            wTable->setItem(i,0,new QTableWidgetItem(unArchi.eventDebut.equipement));
            wTable->setItem(i,1,new QTableWidgetItem(unArchi.eventDebut.dateheure.toString("dd/MM/yyyy hh:mm:ss")));
            wTable->setItem(i,2,new QTableWidgetItem(unArchi.eventFin.dateheure.toString("dd/MM/yyyy hh:mm:ss")));
            wTable->setItem(i,3,new QTableWidgetItem(secToString(unArchi.duree)));



            i++;
        }

        wTable->show();
        n++;
    }
}

void Depouillement::clickOnExport()
{
    QString sPath=QFileDialog::getExistingDirectory(this,QString("Sélectionner le répertoire d'export"),mRepExport);
    if(QDir(sPath).exists())
    {
        mRepExport=sPath;
        mSettings->setValue("ExportPath",mRepExport);
    }
    else
    {
        if(!sPath.isEmpty())
            QMessageBox::warning(this,"Export impossible",QString("Le répertoire %1 est invalide").arg(sPath));

        return;
    }

    exportCSV();
    exportHTML();
    exportGPX();

    QDesktopServices::openUrl(mRepExport);
}

void Depouillement::showProfils()
{
    QListIterator<QList<stProfil>>itGlobal(mListProfils);
    QList<stProfil>uneListe;
    while (itGlobal.hasNext()) {
        uneListe=itGlobal.next();
        QListIterator<stProfil>itProfil(uneListe);
        stProfil unProfil;
        while (itProfil.hasNext()) {
            unProfil=itProfil.next();

        }

    }
}

QList<Depouillement::stProfil> Depouillement::findProfilsFromEvents(QString sEquipement)
{
    LogbookModel::stLogbookData eventDebut;
    LogbookModel::stLogbookData eventFin;
    stProfil profil;
    QList<stProfil> listProfils;
    LogbookModel::stLogbookData unEvent;


    QPair<int,int>bornes=findBornesProfilNumber(sEquipement);
    for(int i=bornes.first;i<=bornes.second;i++)
    {
        bool bFoundD=false;
        bool bFoundF=false;
        QListIterator<LogbookModel::stLogbookData>itLog(mListEvent);
        while (itLog.hasNext()) {
            unEvent=itLog.next();
            if(unEvent.equipement==sEquipement && unEvent.profil==i)
            {
                if(unEvent.titre.contains(QString("Début")))
                {
                    eventDebut=unEvent;
                    bFoundD=true;
                }

                if(unEvent.titre.contains(QString("Fin")))
                {
                    eventFin=unEvent;
                    bFoundF=true;
                }


            }
        }
        if(bFoundD&&bFoundF)
        {
            profil.eventDebut=eventDebut;
            profil.eventFin=eventFin;
            profil.duree=eventDebut.dateheure.secsTo(eventFin.dateheure);
            profil.distance=eventDebut.position.distanceTo(eventFin.position);


            listProfils.append(profil);
        }

    }

    return listProfils;
}

int Depouillement::findSippicanNumberFomEvents()
{
    QListIterator<LogbookModel::stLogbookData>itLog(mListEvent);
    LogbookModel::stLogbookData unEvent;
    int nSip=0;
    while (itLog.hasNext()) {
        unEvent=itLog.next();
        if(unEvent.typeId==LogbookModel::Sippican)
        {
            if(unEvent.probeNumber>nSip)
                nSip=unEvent.probeNumber;


        }
    }
    return nSip;
}

QList<Depouillement::stArcCont> Depouillement::findArcContFromEvents(QString sEquipement)
{
    LogbookModel::stLogbookData eventDebut;
    LogbookModel::stLogbookData eventFin;
    stArcCont archi;
    QList<stArcCont> listArcCont;
    LogbookModel::stLogbookData unEvent;



    bool bFoundD=false;
    bool bFoundF=false;
    QListIterator<LogbookModel::stLogbookData>itLog(mListEvent);
    while (itLog.hasNext()) {
        unEvent=itLog.next();
        if(unEvent.equipement==sEquipement)
        {
            if(unEvent.titre.contains(QString("Début")))
            {
                eventDebut=unEvent;
                bFoundD=true;
            }

            if(unEvent.titre.contains(QString("Fin")))
            {
                eventFin=unEvent;
                bFoundF=true;
            }
            if(bFoundD&&bFoundF)
            {
                bFoundD=false;
                bFoundF=false;
                archi.eventDebut=eventDebut;
                archi.eventFin=eventFin;
                archi.duree=eventDebut.dateheure.secsTo(eventFin.dateheure);
                listArcCont.append(archi);
            }

        }
    }




    return listArcCont;
}

QPair<int, int> Depouillement::findBornesProfilNumber(QString sEquipement)
{
    QListIterator<LogbookModel::stLogbookData>itLog(mListEvent);
    LogbookModel::stLogbookData unEvent;
    int nProfilMin=0;
    int nProfilMax=0;


    bool bFirst=true;
    while (itLog.hasNext()) {
        unEvent=itLog.next();

        if(unEvent.equipement==sEquipement)
        {
            if(bFirst)
            {
                nProfilMin=unEvent.profil;
                nProfilMax=nProfilMin;
                bFirst=false;
            }
            if(unEvent.profil<nProfilMin)
                nProfilMin=unEvent.profil;
            if(unEvent.profil>nProfilMax)
                nProfilMax=unEvent.profil;
        }
    }
    return qMakePair(nProfilMin,nProfilMax);
}



QString Depouillement::metersToNMString(double meters)
{
    double dNM=meters/1852;
    QString sNM=QString::number(dNM,'f',1);
    return sNM;
}

void Depouillement::analyseData()
{
    mNumSippican=findSippicanNumberFomEvents();
    ui->l_Sippican->setText(QString("Nombre de sondes Sippican utilisées: %1").arg(mNumSippican));

    mListResumeArcCont.clear();
    mListResumeProfils.clear();
    QListIterator<AnaProfils*>itWidget(mListProfilsWidgets);
    int i=0;
    while(itWidget.hasNext())
    {
        mLayoutProfils->removeWidget(itWidget.next());
        delete mListProfilsWidgets[i];
        i++;

    }
    mListProfilsWidgets.clear();

    QListIterator<AnaArchiCont*>itWiArc(mListArchiWidgets);
    i=0;
    while(itWiArc.hasNext())
    {
        mLayoutArcCont->removeWidget(itWiArc.next());
        delete mListArchiWidgets[i];
        i++;

    }
    mListProfilsWidgets.clear();
    mListArchiWidgets.clear();

    QListIterator<QList<stProfil>>itGlobal(mListProfils);
    QList<stProfil>uneListe;

    while(itGlobal.hasNext())
    {
        uneListe=itGlobal.next();
        stResumeProfil unResume;
        bool bFirst=true;
        QListIterator<stProfil>itProfil(uneListe);
        stProfil unProfil;

        while(itProfil.hasNext())
        {
            unProfil=itProfil.next();
            if(bFirst)
            {
                unResume.sEquipement=unProfil.eventDebut.equipement;
                unResume.nTotalProfils=uneListe.size();
                bFirst=false;
            }
            unResume.distanceTotale=unResume.distanceTotale+(unProfil.distance/1852);
            unResume.dureeTotale=unResume.dureeTotale+unProfil.duree;


        }


        mListResumeProfils.append(unResume);




    }

    QListIterator<QList<stArcCont>>itGlobArc(mListArcCont);
    QList<stArcCont>uneListeA;

    while(itGlobArc.hasNext())
    {
        uneListeA=itGlobArc.next();
        stResumeArcCont unResume;

        QListIterator<stArcCont>itArc(uneListeA);
        stArcCont unArchi;

        bool bFirst=true;
        while(itArc.hasNext())
        {
            unArchi=itArc.next();
            if(bFirst)
            {
                unResume.sEquipement=unArchi.eventDebut.equipement;

                bFirst=false;
            }
            unResume.dureeTotale=unResume.dureeTotale+unArchi.duree;
        }
        mListResumeArcCont.append(unResume);

    }


    if(mListResumeProfils.size()>0)
    {
        QListIterator<stResumeProfil>itResPro(mListResumeProfils);
        stResumeProfil unResume;

        while (itResPro.hasNext()) {
            unResume=itResPro.next();

            AnaProfils* gbProfil=new AnaProfils();

            gbProfil->setName(unResume.sEquipement);
            gbProfil->setDuree(QString("Duree totale d'acquisition %1: %2").arg(unResume.sEquipement,secToString(unResume.dureeTotale)));
            gbProfil->setNombre(QString("Nombre de profil: %1").arg(unResume.nTotalProfils));
            gbProfil->setDistance(QString("Longueur totale des profils: %1 milles").arg(QString::number(unResume.distanceTotale,'f',1)));
            mListProfilsWidgets.append(gbProfil);

        }


    }

    if(mListResumeArcCont.size()>0)
    {
        QListIterator<stResumeArcCont>itResArc(mListResumeArcCont);
        stResumeArcCont unResume;
        while (itResArc.hasNext()) {
            unResume=itResArc.next();

            AnaArchiCont* gbArcCont=new AnaArchiCont();
            gbArcCont->setName(unResume.sEquipement);
            gbArcCont->setDuree(QString("Duree totale d'acquisition %1: %2").arg(unResume.sEquipement,secToString(unResume.dureeTotale)));
            mListArchiWidgets.append(gbArcCont);

        }


    }

    gestionLayouts();


}

void Depouillement::gestionLayouts()
{


    QListIterator<AnaProfils*>it(mListProfilsWidgets);
    while (it.hasNext()) {

        mLayoutProfils->addWidget(it.next());

    }

    ui->gb_Profils->setLayout(mLayoutProfils);

    QListIterator<AnaArchiCont*>it2(mListArchiWidgets);
    while (it2.hasNext()) {

        mLayoutArcCont->addWidget(it2.next());

    }

    ui->gb_ArcCont->setLayout(mLayoutArcCont);


}

void Depouillement::exportCSV()
{
    QListIterator<LogbookModel::stLogbookData>it(mListEvent);
    LogbookModel::stLogbookData unEvent;

    QString sUnLog;
    QString sUnSip;
    QString sUnEvent;
    QDir dir;
    dir.mkpath(QString("%1/CSV").arg(mRepExport));
    QFile ficLogbook (QString("%1/CSV/Logbook_%2.csv").arg(mRepExport,mCurrentCruise.sNom));
    QFile ficSippican(QString("%1/CSV/Sippican_%2.csv").arg(mRepExport,mCurrentCruise.sNom));
    QFile ficEvenement(QString("%1/CSV/Evenements_%2.csv").arg(mRepExport,mCurrentCruise.sNom));

    if(!ficLogbook.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Erreur d'ouverture logbook.csv";
        return;
    }
    if(!ficSippican.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Erreur d'ouverture Sippicans.csv";
        return;
    }
    if(!ficEvenement.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Erreur d'ouverture Evenement.csv";
        return;
    }

    QTextStream fluxLogBook(&ficLogbook);
    fluxLogBook.setCodec("UTF-8");
    fluxLogBook<<"Date/Heure;Equipement;Titre;Position;COG;SOG;Sonde;Commentaires;\r\n";

    QTextStream fluxSippican(&ficSippican);
    fluxSippican.setCodec("UTF-8");
    fluxSippican<<QString("Date/Heure;Position;COG;SOG;Sonde;Célérité à 3m;Célérimètre;Célérité SBE;Salinité;Température;Type de Sonde;Fichier ASVP;Fichier SIS;Heure de chargement;Commentaires;\r\n").toUtf8();

    QTextStream fluxEvent(&ficEvenement);
    fluxEvent.setCodec("UTF-8");
    fluxEvent<<"Date/Heure;Titre;Position;COG;SOG;Sonde;Commentaires;\r\n";


    while (it.hasNext()) {
        unEvent=it.next();
        //Dateheure;Equipement;Titre;COG;SOG;Sonde;Commentaire;
        sUnLog=QString("%1;%2;%3;%4;%5;%6;%7;%8\r\n").arg(unEvent.dateheure.toString("dd/MM/yyyy hh:mm:ss"),unEvent.equipement,unEvent.titre,unEvent.position.toString(QGeoCoordinate::Degrees),
                                                          QString::number(unEvent.cog,'f',1),QString::number(unEvent.sog,'f',1),QString::number(unEvent.sonde,'f',1),unEvent.commentaire);
        fluxLogBook<<sUnLog;

        if(unEvent.typeId==LogbookModel::Sippican)
        {
            sUnSip=QString("%1;%2;%3;%4;%5;%6;%7;%8;%9;%10;%11;%12;%13;%14;%15\r\n")
                    .arg(unEvent.dateheure.toString("dd/MM/yyyy hh:mm:ss")).arg(unEvent.position.toString(QGeoCoordinate::Degrees)).arg(QString::number(unEvent.cog))
                    .arg(QString::number(unEvent.sog,'f',1)).arg(QString::number(unEvent.sonde,'f',1)).arg(QString::number(unEvent.celSippican,'f',1)).arg(QString::number(unEvent.celerimetre,'f',1)).arg(QString::number(unEvent.celSBE,'f',1))
                    .arg(QString::number(unEvent.salinite,'f',1)).arg(QString::number(unEvent.tempSBE,'f',1)).arg(unEvent.typeSippican).arg(unEvent.ficASVP).arg(unEvent.ficSIS)
                    .arg(unEvent.dateheureSIS.toString("dd/MM/yyyy hh:mm:ss")).arg(unEvent.commentaire);
            fluxSippican<<sUnSip;

        }

        if(unEvent.typeId==LogbookModel::Ponctuel)
        {
            sUnEvent=QString("%1;%2;%3;%4;%5;%6;%7\r\n").arg(unEvent.dateheure.toString("dd/MM/yyyy hh:mm:ss"),unEvent.titre,unEvent.position.toString(QGeoCoordinate::Degrees),
                                                             QString::number(unEvent.cog,'f',1),QString::number(unEvent.sog,'f',1),QString::number(unEvent.sonde,'f',1),unEvent.commentaire);
            fluxEvent<<sUnEvent;

        }


    }

    QListIterator<QList<stProfil>>itGlobal(mListProfils);
    QList<stProfil>uneListe;
    int n=0;
    while(itGlobal.hasNext())
    {
        uneListe=itGlobal.next();

        QString sEq=uneListe.first().eventDebut.equipement.remove(" ");
        QFile ficProfil(QString("%1/CSV/Profil_%2_%3.csv").arg(mRepExport,sEq,mCurrentCruise.sNom));
        if(!ficProfil.open(QIODevice::WriteOnly))
        {
            qDebug()<<QString("Erreur d'ouverture Profil_%1.csv").arg(sEq);
            return;
        }

        QTextStream fluxProfil(&ficProfil);
        fluxProfil.setCodec("UTF-8");

        fluxProfil<<QString("N° de profil;Fichier début;Fichier fin;Heure début;Heure Fin;Position début;Position fin;Durée;Distance(NM)\r\n").toUtf8();
        QString sPro;

        QListIterator<stProfil>itProfil(uneListe);
        stProfil unProfil;

        while(itProfil.hasNext())
        {
            unProfil=itProfil.next();

            fluxProfil<<QString("%1;%2;%3;%4;%5;%6;%7;%8;%9\r\n").arg(QString::number(unProfil.eventDebut.profil),unProfil.eventDebut.ficProfil,unProfil.eventFin.ficProfil,
                                                                      unProfil.eventDebut.dateheure.toString("dd/MM/yyyy hh:mm:ss"),unProfil.eventFin.dateheure.toString("dd/MM/yyyy hh:mm:ss"),
                                                                      unProfil.eventDebut.position.toString(QGeoCoordinate::Degrees),unProfil.eventFin.position.toString(QGeoCoordinate::Degrees),
                                                                      secToString(unProfil.duree),metersToNMString(unProfil.distance));


        }

        fluxProfil<<"\r\n";
        fluxProfil<<QString("Nombre total de profils;;;;;Durée Totale;Distance Totale\r\n").toUtf8();
        fluxProfil<<QString("%1;;;;;;;%2;%3\r\n").arg(QString::number(mListResumeProfils[n].nTotalProfils),secToString(mListResumeProfils[n].dureeTotale),QString::number(mListResumeProfils[n].distanceTotale,'f',2));


        n++;
        ficProfil.close();

    }



    QListIterator<QList<stArcCont>>itGlobalAr(mListArcCont);
    QList<stArcCont>uneListeAr;
    int m=0;
    while(itGlobalAr.hasNext())
    {
        uneListeAr=itGlobalAr.next();

        QString sEq=uneListeAr.first().eventDebut.equipement.remove(" ");
        QFile ficArchi(QString("%1/CSV/Archivage_%2_%3.csv").arg(mRepExport,sEq,mCurrentCruise.sNom));
        if(!ficArchi.open(QIODevice::WriteOnly))
        {
            qDebug()<<QString("Erreur d'ouverture Archivage_%1.csv").arg(sEq);
            return;
        }

        QTextStream fluxArchi(&ficArchi);
        fluxArchi.setCodec("UTF-8");

        fluxArchi<<QString("Heure début;Heure Fin;Position début;Position fin;Durée\r\n").toUtf8();


        QListIterator<stArcCont>itArc(uneListeAr);
        stArcCont unArcCont;

        while(itArc.hasNext())
        {
            unArcCont=itArc.next();

            fluxArchi<<QString("%1;%2;%3;%4;%5\r\n").arg(unArcCont.eventDebut.dateheure.toString("dd/MM/yyyy dd:mm:ss"),unArcCont.eventFin.dateheure.toString("dd/MM/yyyy dd:mm:ss"),
                                                         unArcCont.eventDebut.position.toString(QGeoCoordinate::Degrees),unArcCont.eventFin.position.toString(QGeoCoordinate::Degrees),
                                                         secToString(unArcCont.duree));


        }

        fluxArchi<<"\r\n";
        fluxArchi<<QString(";;;;;Durée Totale\r\n").toUtf8();
        fluxArchi<<QString(";;;;;%1\r\n").arg(secToString(mListResumeArcCont[m].dureeTotale));



        m++;
        ficArchi.close();

    }






    ficLogbook.close();
    ficSippican.close();
    ficEvenement.close();


}

void Depouillement::exportHTML()
{
    QDir dir;
    dir.mkpath(QString("%1/HTML").arg(mRepExport));
    QFile ficReport(QString("%1/Logbook_%2.html").arg(mRepExport,mCurrentCruise.sNom));
    QFile ficSippican(QString("%1/HTML/Sippican_%2.html").arg(mRepExport,mCurrentCruise.sNom));
    QFile ficEvent(QString("%1/HTML/Evenements_%2.html").arg(mRepExport,mCurrentCruise.sNom));

    if(!ficReport.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Erreur d'ouverture Rapport.html";
        return;
    }

    if(!ficSippican.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Erreur d'ouverture Sippican.html";
        return;
    }


    if(!ficEvent.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Erreur d'ouverture Evenement.html";
        return;
    }

    QTextStream fluxReport(&ficReport);
    fluxReport.setCodec("UTF-8");
    QTextStream fluxSippican(&ficSippican);
    fluxSippican.setCodec("UTF-8");
    QTextStream fluxEvents(&ficEvent);
    fluxEvents.setCodec("UTF-8");


    fluxReport<<QString("<HTML><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><HEAD><title>Résumé de la campagne "
                        +mCurrentCruise.sNom+"</title><style>"
                                             "table {margin: 0 auto;}td {background-color: #e0e0e0;padding: 2px 5px;font-family: 'Microsoft Sans Serif';"
                                             "border-radius: 3px;}pre {font-family: 'Microsoft Sans Serif';}tr.quarter td {background-color: #277db5;color: white;}"
                                             ".header {background-color: #202020;color: white;font-weight: bold;text-align: center;}tr.event:nth-child(2n+1) td "
                                             "{background-color: #b0b0b0;}tr.critialEvent td {background-color: red;}tr.critialEvent td:last-child "
                                             "{text-align: center;}tr.highLevelEvent td {background-color: #97F786;}tr.navErrorEvent td {background-color: yellow; } "
                                             "@media print {body {-webkit-print-color-adjust: exact;}}"
                                             "</style></HEAD><BODY BGCOLOR=\"#FFFFFF\">"
                                             "<table width=\"600px\">"
                                             "<tr><th colspan=\"2\" class=\"header\">Campagne "+mCurrentCruise.sNom+"</th></tr>"
                                             "<tr><td>Navire</td><td>"+mCurrentCruise.sNavire+"</td></tr>"
                                             "<tr><td>Zone Géographique</td><td>"+mCurrentCruise.sZone+"</td></tr>"
                                             "<tr><td>Date de début de campagne</td><td>"+mCurrentCruise.dateDebut.toString("dd/MM/yyyy")+"</td></tr>"
                                             "<tr><td>Date de fin de campagne</td><td>"+mCurrentCruise.dateFin.toString("dd/MM/yyyy")+"</td></tr>"
                                             "<tr><td>Chef(fe) de mission</td><td>"+mCurrentCruise.sChefMission+"</td></tr>"
                                             "<tr><td>Opérateurs</td><td>"+mCurrentCruise.sOperateurs+"</td></tr>"
                                             "</table>"
                                             "<br /><br />");

    QListIterator<stResumeProfil>itPr(mListResumeProfils);
    stResumeProfil unResumePro;
    while(itPr.hasNext())
    {
        unResumePro=itPr.next();

        fluxReport<<QString("<table width=\"600px\"><tr><th colspan=\"2\" class=\"header\">"+unResumePro.sEquipement+"</th></tr>"
                    "<tr><td>Nombre de profils</td><td>"+QString::number(unResumePro.nTotalProfils)+"</td></tr>"
                    "<tr><td>Durée totale d'acquisition</td><td>"+secToString(unResumePro.dureeTotale)+"</td></tr>"
                    "<tr><td>Distance totale (NM)</td><td>"+QString::number(unResumePro.distanceTotale,'f',1)+" milles</td></tr>"
                    "</table><br /><br />");
    }

    QListIterator<stResumeArcCont>itAr(mListResumeArcCont);
    stResumeArcCont unResumeAr;
    while(itAr.hasNext())
    {
        unResumeAr=itAr.next();
        fluxReport<<QString("<table width=\"600px\"><tr><th colspan=\"2\" class=\"header\">"+unResumeAr.sEquipement+"</th></tr>"
                    "<tr><td>Durée totale d'acquisition</td><td>"+secToString(unResumeAr.dureeTotale)+"</td></tr>"
                    "</table><br /><br />");

    }

    fluxReport<<QString("<table width=\"600px\"><tr><th colspan=\"2\" class=\"header\"> SIPPICAN </th></tr>"
                "<tr><td>Nombre de sondes Sippican utilisées</td><td>"+QString::number(findSippicanNumberFomEvents())+"</td></tr>"
                "</table><br /><br />");


    fluxSippican<<QString("<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><head><title>Tableau Sippican </title>"
                 "<style>table {margin: 5px;}td {background-color: #e0e0e0;padding: 2px 5px;font-family: 'Microsoft Sans Serif';border-radius: 3px;}pre {font-family: 'Microsoft Sans Serif';}tr.quarter td {background-color: #277db5;color: white;}.header {background-color: #202020;color: white;font-weight: bold;text-align: center;}tr.event:nth-child(2n+1) td {background-color: #b0b0b0;}tr.critialEvent td {background-color: red;}tr.critialEvent td:last-child {text-align: center;}tr.highLevelEvent td {background-color: #97F786;}tr.navErrorEvent td {background-color: yellow; } @media print {body {-webkit-print-color-adjust: exact;}}"
                 "</style></head><body><table><tr><td class=\"header\" style=\"text-align: right;\">Campagne</td><td>"+mCurrentCruise.sNom+"</td></tr>"
                 "<tr><td class=\"header\" style=\"text-align: center;\">Tableau Sippican</td></tr>"
                  "<table><thead><tr>"
                 "<table><thead><tr><td class=\"header\" style=\"min-width: 200px\">Date/Heure</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Position</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">COG</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">SOG</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Sonde</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Célérité à 3m</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Célérimètre</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Célérité SBE</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Salinité</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Température</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Type de sonde</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Fichier ASVP</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Fichier SIS</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Heure de chargement</td>"
                          "<td class=\"header\" style=\"min-width: 100px\">Commentaires</td>"
                          "</tr></thead>");

    fluxEvents<<QString("<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><head><title>Tableau Evénements </title>"
                        "<style>table {margin: 5px;}td {background-color: #e0e0e0;padding: 2px 5px;font-family: 'Microsoft Sans Serif';border-radius: 3px;}pre {font-family: 'Microsoft Sans Serif';}tr.quarter td {background-color: #277db5;color: white;}.header {background-color: #202020;color: white;font-weight: bold;text-align: center;}tr.event:nth-child(2n+1) td {background-color: #b0b0b0;}tr.critialEvent td {background-color: red;}tr.critialEvent td:last-child {text-align: center;}tr.highLevelEvent td {background-color: #97F786;}tr.navErrorEvent td {background-color: yellow; } @media print {body {-webkit-print-color-adjust: exact;}}"
                        "</style></head><body><table><tr><td class=\"header\" style=\"text-align: right;\">Campagne</td><td>"+mCurrentCruise.sNom+"</td></tr>"
                        "<tr><td class=\"header\" style=\"text-align: center;\">Tableau Evénements</td></tr>"
                         "<table><thead><tr>"
                        "<table><thead><tr><td class=\"header\" style=\"min-width: 200px\">Date/Heure</td>"
                                 "<td class=\"header\" style=\"min-width: 100px\">Titre</td>"
                                 "<td class=\"header\" style=\"min-width: 100px\">Position</td>"
                                 "<td class=\"header\" style=\"min-width: 100px\">COG</td>"
                                 "<td class=\"header\" style=\"min-width: 100px\">SOG</td>"
                                 "<td class=\"header\" style=\"min-width: 100px\">Sonde</td>"
                                 "<td class=\"header\" style=\"min-width: 100px\">Commentaires</td>"
                                 "</tr></thead>");

    QListIterator<LogbookModel::stLogbookData>itEvent(mListEvent);
    LogbookModel::stLogbookData unEvent;

    fluxReport<<QString("<table><thead><tr><td class=\"header\" style=\"min-width: 200px\">Date/Heure</td>"
    "<td class=\"header\" style=\"min-width: 100px\">Equipement</td>"
    "<td class=\"header\" style=\"min-width: 100px\">Titre</td>"
    "<td class=\"header\" style=\"min-width: 100px\">Position</td>"
    "<td class=\"header\" style=\"min-width: 100px\">COG</td>"
    "<td class=\"header\" style=\"min-width: 100px\">SOG</td>"
    "<td class=\"header\" style=\"min-width: 100px\">Sonde</td>"
    "<td class=\"header\" style=\"min-width: 100px\">Commentaires</td>"
    "</tr></thead>");


    while(itEvent.hasNext())
    {
        unEvent=itEvent.next();
        fluxReport<<QString("<tr class=\""+unEvent.equipement+"\"><td>"+unEvent.dateheure.toString("dd/MM/yyyy hh:mm:ss")+"</td>"
                             "<td>"+unEvent.equipement+"</td>"
                            "<td>"+unEvent.titre+"</td>"
                            "<td>"+unEvent.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere)+"</td>"
                            "<td>"+QString::number(unEvent.cog,'f',1)+"</td>"
                            "<td>"+QString::number(unEvent.sog,'f',1)+"</td>"
                            "<td>"+QString::number(unEvent.sonde,'f',1)+"</td>"
                            "<td>"+unEvent.commentaire+"</td></tr>");

        if(unEvent.typeId==LogbookModel::Sippican)
        {
            fluxSippican<<QString("<td>"+unEvent.dateheure.toString("dd/MM/yyyy hh:mm:ss")+"</td>"
                                  "<td>"+unEvent.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere)+"</td>"
                                  "<td>"+QString::number(unEvent.cog,'f',1)+"°</td>"
                                  "<td>"+QString::number(unEvent.sog,'f',1)+" nd</td>"
                                  "<td>"+QString::number(unEvent.sonde,'f',1)+" m</td>"
                                  "<td>"+QString::number(unEvent.celSippican,'f',1)+" m/s</td>"
                                  "<td>"+QString::number(unEvent.celerimetre,'f',1)+" m/s</td>"
                                  "<td>"+QString::number(unEvent.celSBE,'f',1)+" m/s</td>"
                                  "<td>"+QString::number(unEvent.salinite,'f',1)+" psu</td>"
                                  "<td>"+QString::number(unEvent.tempSBE,'f',1)+" °C</td>"
                                  "<td>"+unEvent.typeSippican+"</td>"
                                  "<td>"+unEvent.ficASVP+"</td>"
                                  "<td>"+unEvent.ficSIS+"</td>"
                                  "<td>"+unEvent.dateheureSIS.toString("dd/MM/yyyy hh:mm:ss")+"</td>"
                                  "<td>"+unEvent.commentaire+"</td></tr>");
        }

        if(unEvent.typeId==LogbookModel::Ponctuel)
        {
            fluxEvents<<QString("<td>"+unEvent.dateheure.toString("dd/MM/yyyy hh:mm:ss")+"</td>"
                                "<td>"+unEvent.titre+"</td>"
                                "<td>"+unEvent.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere)+"</td>"
                                 "<td>"+QString::number(unEvent.cog,'f',1)+"°</td>"
                                 "<td>"+QString::number(unEvent.sog,'f',1)+" nd</td>"
                                 "<td>"+QString::number(unEvent.sonde,'f',1)+" m</td>"
                                 "<td>"+unEvent.commentaire+"<td></tr>");
        }

    }
    fluxReport<<QString("</table><br /><br />");
    fluxSippican<<QString("</table><br /><br />");
    fluxEvents<<QString("</table><br /><br />");

    fluxReport<<QString("</BODY></HTML>");
    fluxSippican<<QString("</BODY></HTML>");
    fluxEvents<<QString("</BODY></HTML>");

    ficReport.close();
    ficSippican.close();
    ficEvent.close();

    QListIterator<QList<stProfil>>itGlobal(mListProfils);
    QList<stProfil>uneListe;
    int n=0;
    while(itGlobal.hasNext())
    {
        uneListe=itGlobal.next();

        QString sEq=uneListe.first().eventDebut.equipement.remove(" ");
        QFile ficProfil(QString("%1/HTML/Profils_%2_%3.html").arg(mRepExport,sEq,mCurrentCruise.sNom));
        if(!ficProfil.open(QIODevice::WriteOnly))
        {
            qDebug()<<QString("Erreur d'ouverture Profils_%1.html").arg(sEq);
            return;
        }

        QTextStream fluxProfil(&ficProfil);
        fluxProfil.setCodec("UTF-8");

        //fluxProfil<<QString("N° de profil;Fichier début;Fichier fin;Heure début;Heure Fin;Position début;Position fin;Durée;Distance(NM)\r\n").toUtf8();

        fluxProfil<<QString("<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><head><title>Tableau Profils</title>"
                            "<style>table {margin: 5px;}td {background-color: #e0e0e0;padding: 2px 5px;font-family: 'Microsoft Sans Serif';border-radius: 3px;}pre {font-family: 'Microsoft Sans Serif';}tr.quarter td {background-color: #277db5;color: white;}.header {background-color: #202020;color: white;font-weight: bold;text-align: center;}tr.event:nth-child(2n+1) td {background-color: #b0b0b0;}tr.critialEvent td {background-color: red;}tr.critialEvent td:last-child {text-align: center;}tr.highLevelEvent td {background-color: #97F786;}tr.navErrorEvent td {background-color: yellow; } @media print {body {-webkit-print-color-adjust: exact;}}"
                            "</style></head><body><table><tr><td class=\"header\" style=\"text-align: right;\">Profils</td><td>"+uneListe.first().eventDebut.equipement+"</td></tr>"
                             "<table><thead><tr>"
                            "<table><thead><tr><td class=\"header\" style=\"min-width: 200px\">N° de profil</td>"
                                     "<td class=\"header\" style=\"min-width: 100px\">Fichier début</td>"
                                     "<td class=\"header\" style=\"min-width: 100px\">Fichier fin</td>"
                                     "<td class=\"header\" style=\"min-width: 100px\">Heure début</td>"
                                     "<td class=\"header\" style=\"min-width: 100px\">heure fin</td>"
                                     "<td class=\"header\" style=\"min-width: 100px\">Position début</td>"
                                     "<td class=\"header\" style=\"min-width: 100px\">Position fin</td>"
                                     "<td class=\"header\" style=\"min-width: 100px\">Durée</td>"
                                     "<td class=\"header\" style=\"min-width: 100px\">Distance</td>"
                                     "</tr></thead>");
        QString sPro;

        QListIterator<stProfil>itProfil(uneListe);
        stProfil unProfil;

        while(itProfil.hasNext())
        {
            unProfil=itProfil.next();

            fluxProfil<<QString("<td>"+QString::number(unProfil.eventDebut.profil)+"</td>"
                                "<td>"+unProfil.eventDebut.ficProfil+"</td>"
                                "<td>"+unProfil.eventFin.ficProfil+"</td>"
                                "<td>"+unProfil.eventDebut.dateheure.toString("dd/MM/yyyy hh:mm:ss")+"</td>"
                                "<td>"+unProfil.eventFin.dateheure.toString("dd/MM/yyyy hh:mm:ss")+"</td>"
                                "<td>"+unProfil.eventDebut.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere)+"</td>"
                                "<td>"+unProfil.eventFin.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere)+"</td>"
                                "<td>"+secToString(unProfil.duree)+"</td>"
                                "<td>"+metersToNMString(unProfil.distance)+" milles</td></tr>");



        }

        fluxProfil<<QString("</table><br /><br />");

        fluxProfil<< QString("<table width=\"600px\"><tr><th colspan=\"2\" class=\"header\">"+mListResumeProfils[n].sEquipement+"</th></tr>"
                            "<tr><td>Nombre de profils</td><td>"+QString::number(mListResumeProfils[n].nTotalProfils)+"</td></tr>"
                            "<tr><td>Durée totale d'acquisition</td><td>"+secToString(mListResumeProfils[n].dureeTotale)+"</td></tr>"
                            "<tr><td>Distance totale (NM)</td><td>"+QString::number(mListResumeProfils[n].distanceTotale,'f',1)+" milles</td></tr>"
                            "</table><br /><br />");


        fluxProfil<<QString("</BODY></HTML>");

        ficProfil.close();
        n++;
    }



    QListIterator<QList<stArcCont>>itGlobalAr(mListArcCont);
    QList<stArcCont>uneListeAr;
    int m=0;
    while(itGlobalAr.hasNext())
    {
        uneListeAr=itGlobalAr.next();

        QString sEq=uneListeAr.first().eventDebut.equipement.remove(" ");
        QFile ficArchi(QString("%1/HTML/Archivage_%2_%3.html").arg(mRepExport,sEq,mCurrentCruise.sNom));
        if(!ficArchi.open(QIODevice::WriteOnly))
        {
            qDebug()<<QString("Erreur d'ouverture Archivage_%1.html").arg(sEq);
            return;
        }

        QTextStream fluxArchi(&ficArchi);
        fluxArchi.setCodec("UTF-8");

        fluxArchi<<QString("<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><head><title>Tableau Archivage </title>"
                           "<style>table {margin: 5px;}td {background-color: #e0e0e0;padding: 2px 5px;font-family: 'Microsoft Sans Serif';border-radius: 3px;}pre {font-family: 'Microsoft Sans Serif';}tr.quarter td {background-color: #277db5;color: white;}.header {background-color: #202020;color: white;font-weight: bold;text-align: center;}tr.event:nth-child(2n+1) td {background-color: #b0b0b0;}tr.critialEvent td {background-color: red;}tr.critialEvent td:last-child {text-align: center;}tr.highLevelEvent td {background-color: #97F786;}tr.navErrorEvent td {background-color: yellow; } @media print {body {-webkit-print-color-adjust: exact;}}"
                           "</style></head><body><table><tr><td class=\"header\" style=\"text-align: right;\">Archivage</td><td>"+uneListeAr.first().eventDebut.equipement+"</td></tr>"
                            "<table><thead><tr>"
                           "<table><thead><tr><td class=\"header\" style=\"min-width: 200px\">Heure début</td>"
                                    "<td class=\"header\" style=\"min-width: 100px\">heure fin</td>"
                                    "<td class=\"header\" style=\"min-width: 100px\">Position début</td>"
                                    "<td class=\"header\" style=\"min-width: 100px\">Position fin</td>"
                                    "<td class=\"header\" style=\"min-width: 100px\">Durée</td>"
                                    "</tr></thead>");


        QListIterator<stArcCont>itArc(uneListeAr);
        stArcCont unArcCont;

        while(itArc.hasNext())
        {
            unArcCont=itArc.next();

            fluxArchi<<QString("<td>"+unArcCont.eventDebut.dateheure.toString("dd/MM/yyyy hh:mm:ss")+"</td>"
                               "<td>"+unArcCont.eventFin.dateheure.toString("dd/MM/yyyy hh:mm:ss")+"</td>"
                               "<td>"+unArcCont.eventDebut.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere)+"</td>"
                               "<td>"+unArcCont.eventFin.position.toString(QGeoCoordinate::DegreesMinutesWithHemisphere)+"</td>"
                               "<td>"+secToString(unArcCont.duree)+"</td></tr>");


        }

        fluxArchi<<QString("</table><br /><br />");


        fluxArchi<< QString("<table width=\"600px\"><tr><th colspan=\"2\" class=\"header\">"+mListResumeArcCont[m].sEquipement+"</th></tr>"
                            "<tr><td>Durée totale d'acquisition</td><td>"+secToString(mListResumeArcCont[m].dureeTotale)+"</td></tr>"
                            "</table><br /><br />");

        fluxArchi<<QString("</BODY></HTML>");

        ficArchi.close();



        m++;


    }



}

void Depouillement::exportGPX()
{
    QDir dir;
    dir.mkpath(QString("%1/GPX").arg(mRepExport));
    QFile ficGPX (QString("%1/GPX/%2.gpx").arg(mRepExport,mCurrentCruise.sNom));
    if(!ficGPX.open(QIODevice::WriteOnly))
    {
        qDebug()<<QString("Erreur d'ouverture %1").arg(QString("%1/%2.gpx").arg(mRepExport,mCurrentCruise.sNom));
        return;
    }

    QDomDocument *doc=new QDomDocument("DomDocument"); // creation du document
    QTextStream out(&ficGPX);
    out.setCodec("UTF-8");
    QDomElement gpx =doc->createElement("gpx"); // premiere balise du fichier gpx
  //xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd"
//xmlns:opencpn="http://www.opencpn.org">
    gpx.setAttribute("version","1.1");
    gpx.setAttribute ("creator","GENAVIR");
    gpx.setAttribute ("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
    gpx.setAttribute ("xmlns","http://www.topografix.com/GPX/1/1");
    gpx.setAttribute("xmlns:gpxx","http://www.garmin.com/xmlschemas/GpxExtensions/v3");
    gpx.setAttribute ("xsi:schemaLocation","http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
    doc->appendChild(gpx);




    // root node
    QDomElement wpt = doc->createElement("wpt");
    QListIterator<LogbookModel::stLogbookData>itEvent(mListEvent);
    LogbookModel::stLogbookData unEvent;
    while(itEvent.hasNext())
    {
        unEvent=itEvent.next();
        if(unEvent.typeId==LogbookModel::Sippican)
        {
            QDomElement wpt = doc->createElement("wpt");
            QString sLat=QString::number(unEvent.position.latitude(),'f',8);
            QString sLong=QString::number(unEvent.position.longitude(),'f',8);
            wpt.setAttribute("lat",sLat);
            wpt.setAttribute("lon",sLong);

            QDomElement time = doc->createElement("time");
            time.appendChild(doc->createTextNode(unEvent.dateheure.toString("yyyy-MM-ddThh:mm:ssZ")));
            wpt.appendChild(time);

            QDomElement name = doc->createElement("name");
            name.appendChild(doc->createTextNode(QString("Sip_%1").arg(QString::number(unEvent.probeNumber))));
            wpt.appendChild(name);

            QDomElement desc = doc->createElement("desc");
            desc.appendChild(doc->createTextNode(QString("Fichier_%1").arg(unEvent.ficASVP)));
            wpt.appendChild(desc);

            QDomElement sym = doc->createElement("sym");
            sym.appendChild(doc->createTextNode(QString("1st-Active-Waypoint")));
            wpt.appendChild(sym);


            gpx.appendChild(wpt);
        }


        if(unEvent.typeId==LogbookModel::Ponctuel)
        {
            QDomElement wpt = doc->createElement("wpt");
            QString sLat=QString::number(unEvent.position.latitude(),'f',8);
            QString sLong=QString::number(unEvent.position.longitude(),'f',8);
            wpt.setAttribute("lat",sLat);
            wpt.setAttribute("lon",sLong);

            QDomElement time = doc->createElement("time");
            time.appendChild(doc->createTextNode(unEvent.dateheure.toString("yyyy-MM-ddThh:mm:ssZ")));
            wpt.appendChild(time);

            QDomElement name = doc->createElement("name");
            name.appendChild(doc->createTextNode(unEvent.titre));
            wpt.appendChild(name);

            QDomElement desc = doc->createElement("desc");
            desc.appendChild(doc->createTextNode(unEvent.commentaire));
            wpt.appendChild(desc);

            QDomElement sym = doc->createElement("sym");
            sym.appendChild(doc->createTextNode(QString("Info-Info")));
            wpt.appendChild(sym);


            gpx.appendChild(wpt);
        }

        if(unEvent.typeId==LogbookModel::ArcContinu)
        {
            QDomElement wpt = doc->createElement("wpt");
            QString sLat=QString::number(unEvent.position.latitude(),'f',8);
            QString sLong=QString::number(unEvent.position.longitude(),'f',8);
            wpt.setAttribute("lat",sLat);
            wpt.setAttribute("lon",sLong);

            QDomElement time = doc->createElement("time");
            time.appendChild(doc->createTextNode(unEvent.dateheure.toString("yyyy-MM-ddThh:mm:ssZ")));
            wpt.appendChild(time);

            QDomElement name = doc->createElement("name");
            name.appendChild(doc->createTextNode(QString("%1:%2").arg(unEvent.equipement,unEvent.titre)));
            wpt.appendChild(name);

            QDomElement desc = doc->createElement("desc");
            desc.appendChild(doc->createTextNode(unEvent.commentaire));
            wpt.appendChild(desc);

            QDomElement sym = doc->createElement("sym");
            sym.appendChild(doc->createTextNode(QString("Info-Video")));
            wpt.appendChild(sym);


            gpx.appendChild(wpt);
        }

    }

    QListIterator<QList<stProfil>>itGlobal(mListProfils);
    QList<stProfil>uneListe;
    int n=0;
    while(itGlobal.hasNext())
    {
        uneListe=itGlobal.next();


        QListIterator<stProfil>itProfil(uneListe);
        stProfil unProfil;
        int i=0;
        while(itProfil.hasNext())
        {
             unProfil=itProfil.next();
            QDomElement rte = doc->createElement("rte");
            QDomElement proName = doc->createElement("name");
            proName.appendChild(doc->createTextNode(QString("Profil_%1_%2").arg(unProfil.eventDebut.equipement,QString::number(unProfil.eventDebut.profil))));
            rte.appendChild(proName);

            QDomElement rtept = doc->createElement("rtept");
            QString sLat=QString::number(unProfil.eventDebut.position.latitude(),'f',8);
            QString sLong=QString::number(unProfil.eventDebut.position.longitude(),'f',8);
            rtept.setAttribute("lat",sLat);
            rtept.setAttribute("lon",sLong);

            QDomElement time = doc->createElement("time");
            time.appendChild(doc->createTextNode(unProfil.eventDebut.dateheure.toString("yyyy-MM-ddThh:mm:ssZ")));
            rtept.appendChild(time);

            QDomElement name = doc->createElement("name");
            name.appendChild(doc->createTextNode(QString("%1_%2_D").arg(unProfil.eventDebut.equipement,QString::number(unProfil.eventDebut.profil))));
            rtept.appendChild(name);

            QDomElement desc = doc->createElement("desc");
            desc.appendChild(doc->createTextNode(unProfil.eventDebut.commentaire));
            rtept.appendChild(desc);

            QDomElement sym = doc->createElement("sym");
            sym.appendChild(doc->createTextNode(QString("Diamond")));
            rtept.appendChild(sym);
            rte.appendChild(rtept);

            rtept = doc->createElement("rtept");
            sLat=QString::number(unProfil.eventFin.position.latitude(),'f',8);
            sLong=QString::number(unProfil.eventFin.position.longitude(),'f',8);
            rtept.setAttribute("lat",sLat);
            rtept.setAttribute("lon",sLong);

            time = doc->createElement("time");
            time.appendChild(doc->createTextNode(unProfil.eventFin.dateheure.toString("yyyy-MM-ddThh:mm:ssZ")));
            rtept.appendChild(time);

            name = doc->createElement("name");
            name.appendChild(doc->createTextNode(QString("%1_%2_F").arg(unProfil.eventFin.equipement,QString::number(unProfil.eventFin.profil))));
            rtept.appendChild(name);

            desc = doc->createElement("desc");
            desc.appendChild(doc->createTextNode(unProfil.eventFin.commentaire));
            rtept.appendChild(desc);

            sym = doc->createElement("sym");
            sym.appendChild(doc->createTextNode(QString("Diamond")));
            rtept.appendChild(sym);
            rte.appendChild(rtept);


             gpx.appendChild(rte);




            i++;
        }


        n++;


    }






    QString write_Doc=doc->toString();
    out<<write_Doc;
    ficGPX.close();
}

QString Depouillement::secToString(qint64 nSec)
{
    int nHeure=nSec/3600;
    int nMin=(nSec-(nHeure*3600))/60;
    int nSecondes=nSec-(nHeure*3600)-(nMin*60);
    QString sHeures,sMinutes,sSecondes;
    if(nHeure>1)
        sHeures="heures";
    else
        sHeures="heure";
    if(nMin>1)
        sMinutes="minutes";
    else
        sMinutes="minute";
    if(nSecondes>1)
        sSecondes="secondes";
    else
        sSecondes="seconde";
    QString nDuree=QString("%1 %2 %3 %4 %5 %6").arg(QString::number(nHeure),sHeures,QString::number(nMin),sMinutes,QString::number(nSecondes),sSecondes);

    return nDuree;

}



