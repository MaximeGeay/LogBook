#ifndef CUSTOMTABLEMODEL_H
#define CUSTOMTABLEMODEL_H

#include <QStandardItemModel>
#include "logbookmodel.h"
class CustomTableModel : public QStandardItemModel
{
    Q_OBJECT
public:
    CustomTableModel();
    ~CustomTableModel();
    CustomTableModel(int nType);

public slots:
    void setTypeId(int nType);
    void setEventList(QList<LogbookModel::stLogbookData> eventList);
    void clearRows();
    void initModel();

signals:
   void modelIsinit();

private:
    int mTypeId=0;
};

#endif // CUSTOMTABLEMODEL_H
