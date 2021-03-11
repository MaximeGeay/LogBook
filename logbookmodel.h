#ifndef LOGBOOKMODEL_H
#define LOGBOOKMODEL_H

#include <QStandardItemModel>
#include <QtXml>
class LogbookModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit LogbookModel(QObject *parent = nullptr);

signals:
    void xmlError(QString sMsg);

public slots:
    bool initXML(QString sPath);
    void addElement(QDomElement element);

private:
    QDomDocument *mDomEventList;
    bool mXMLFileIsInit=false;
    QString mPathToXML;

    bool majXML();
    bool creeXML(QString sPath);
    bool readXML(QString sPath);

};

#endif // LOGBOOKMODEL_H
