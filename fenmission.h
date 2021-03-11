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
        QString sNom,sNavire,sChefMission,sZone,sOperateurs,sPath;
        QStringList listDataGroups;
        QDate dateDebut,dateFin;
    };

    explicit fenMission(QWidget *parent = nullptr);
    ~fenMission();

    st_Mission getCurrentCruise();

public slots:
    void newCruise();
    void editCruise();
    QString getCurrentCruiseName();


private slots:
    void valider();
    void annuler();
    void setAdmin(int nAdm);

signals:
    void editingFinished();
    void newCruiseSet(QString sNewCruise);

private:
    Ui::fenMission *ui;

    QSettings *mSettings;

    bool mNewCruise=false;




};

#endif // FENMISSION_H
