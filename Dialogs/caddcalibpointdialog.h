#ifndef CADDCALIBPOINTDIALOG_H
#define CADDCALIBPOINTDIALOG_H

#include <QDialog>

class gp_Pnt;

namespace Ui {
class CAddCalibPointDialog;
}

class CAddCalibPointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CAddCalibPointDialog(const gp_Pnt &pos, QWidget *parent);
    ~CAddCalibPointDialog();

public:
    gp_Pnt getGlobalPos() const;
    gp_Pnt getBotPos() const;

private:
    Ui::CAddCalibPointDialog *ui;
};

#endif // CADDCALIBPOINTDIALOG_H
