#ifndef CMOVEBOTTASKDIALOG_H
#define CMOVEBOTTASKDIALOG_H

#include <QDialog>

class gp_Pnt;

namespace Ui {
class CMoveBotTaskDialog;
}

class CMoveBotTaskDialog : public QDialog
{
public:
    explicit CMoveBotTaskDialog(QWidget *parent, const gp_Pnt pos);
    ~CMoveBotTaskDialog();

    gp_Pnt getPos() const;
    gp_Pnt getAngles() const;

private:
    Ui::CMoveBotTaskDialog *ui;
};

#endif // CMOVEBOTTASKDIALOG_H
