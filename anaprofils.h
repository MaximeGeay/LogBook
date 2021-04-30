#ifndef ANAPROFILS_H
#define ANAPROFILS_H

#include <QWidget>

namespace Ui {
class AnaProfils;
}

class AnaProfils : public QWidget
{
    Q_OBJECT

public:
    explicit AnaProfils(QWidget *parent = nullptr);
    ~AnaProfils();

    void setName(QString sName);
    void setDuree(QString sDuree);
    void setNombre(QString sNombre);
    void setDistance(QString sDistance);
    QString getName();

private:
    Ui::AnaProfils *ui;
};

#endif // ANAPROFILS_H
