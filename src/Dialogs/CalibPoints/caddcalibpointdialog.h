#ifndef CADDCALIBPOINTDIALOG_H
#define CADDCALIBPOINTDIALOG_H

#include <QDialog>

namespace GUI_TYPES {
struct SCalibPoint;
}

namespace Ui {
class CAddCalibPointDialog;
}

class CAddCalibPointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CAddCalibPointDialog(QWidget *parent, const GUI_TYPES::SCalibPoint &initData);
    ~CAddCalibPointDialog();

public:
    GUI_TYPES::SCalibPoint getCalibPoint() const;

private:
    Ui::CAddCalibPointDialog *ui;
};

#endif // CADDCALIBPOINTDIALOG_H
