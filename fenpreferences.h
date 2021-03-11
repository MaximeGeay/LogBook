#ifndef FENPREFERENCES_H
#define FENPREFERENCES_H

#include <QWidget>

#include "eventlistmodel.h"
#include "eventsettings.h"

namespace Ui {
class FenPreferences;
}

class FenPreferences : public QWidget
{
    Q_OBJECT

public:
    explicit FenPreferences(QWidget *parent = nullptr);
    ~FenPreferences();

signals:
    void confChanged();
    void xmlError(QString sMsg);
public slots:
    void initFen();
    QList<EventSettings::eventProperties> getEventTypeList();
    void openRepConf();
    QString getLogbookDir();

private slots:
    void clickOnValider();
    void clickOnAnnuler();
    void clickOnAddEvent();
    void clickOnModifyEvent();
    void clickOnRemoveEvent();
    void clickOnSelFicConfig();
    void clickOnSelRepLogbook();
    void majListEvent();
    void showErrors(QString sError);

private:
    Ui::FenPreferences *ui;

    QString mPathToConfXML;
    QString mPathToLogbookDir;
    EventSettings* mEventSettings;
    EventListModel* mEventListModel;




};

#endif // FENPREFERENCES_H
