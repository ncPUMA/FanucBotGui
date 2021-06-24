#ifndef CDRILLBOTTASKDIALOG_H
#define CDRILLBOTTASKDIALOG_H

#include <QDialog>

class gp_Pnt;

namespace Ui {
class CDrillBotTaskDialog;
}

class CDrillBotTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CDrillBotTaskDialog(QWidget *parent, const gp_Pnt pos);
    ~CDrillBotTaskDialog();

    gp_Pnt getPos() const;
    gp_Pnt getAngles() const;

private:
    Ui::CDrillBotTaskDialog *ui;
};

#endif // CDRILLBOTTASKDIALOG_H
