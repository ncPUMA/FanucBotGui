#ifndef CMARKBOTTASKDIALOG_H
#define CMARKBOTTASKDIALOG_H

#include <QDialog>

class gp_Pnt;

namespace Ui {
class CMarkBotTaskDialog;
}

class CMarkBotTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CMarkBotTaskDialog(QWidget *parent, const gp_Pnt pos);
    ~CMarkBotTaskDialog();

    gp_Pnt getPos() const;
    gp_Pnt getAngles() const;

private:
    Ui::CMarkBotTaskDialog *ui;
};

#endif // CMARKBOTTASKDIALOG_H
