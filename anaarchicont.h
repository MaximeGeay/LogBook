#ifndef ANAARCHICONT_H
#define ANAARCHICONT_H

#include <QWidget>

namespace Ui {
class AnaArchiCont;
}

class AnaArchiCont : public QWidget
{
    Q_OBJECT

public:
    explicit AnaArchiCont(QWidget *parent = nullptr);
    ~AnaArchiCont();
    void setName(QString sName);
    void setDuree(QString sDuree);
private:
    Ui::AnaArchiCont *ui;
};

#endif // ANAARCHICONT_H
