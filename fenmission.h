#ifndef FENMISSION_H
#define FENMISSION_H

#include <QWidget>
#include <QDate>


namespace Ui {
class fenMission;
}
class QSettings;
class QVBoxLayout;
class QCheckBox;
class fenMission : public QWidget
{
    Q_OBJECT

public:

    struct st_Mission
    {
        QString sNom,sNavire,sChefMission,sZone,sOperateurs;
        //QStringList listDataGroups;
        QDate dateDebut,dateFin;
    };

    explicit fenMission(QWidget *parent = nullptr);
    ~fenMission();



public slots:
    void newCruise();
    void editCruise();
    QString getCurrentCruiseName();
    void setCurrentCruise(st_Mission currentCruise);
    st_Mission getCurrentCruise();


private slots:
    void valider();
    void annuler();
    void setAdmin(int nAdm);

signals:
    void newCruiseSet(QString sNewCruise);
    void newCruiseDetails(st_Mission);

private:
    Ui::fenMission *ui;

    QSettings *mSettings;

    st_Mission mCurrentCruise;
    bool mNewCruise=false;




};

#endif // FENMISSION_H
