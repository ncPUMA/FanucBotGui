#ifndef CCALIBPOINTSORDERDIALOG_H
#define CCALIBPOINTSORDERDIALOG_H

#include <QDialog>

#include "../../gui_types.h"

namespace Ui {
class CCalibPointsOrderDialog;
}

class CCalibPointsOrderDialogPrivate;

class CCalibPointsOrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CCalibPointsOrderDialog(const std::vector <GUI_TYPES::SCalibPoint> &calibPnts,
                                     QWidget *parent = nullptr);
    ~CCalibPointsOrderDialog();

    std::vector<GUI_TYPES::SCalibPoint> getCalibPoints() const;

private:
    Ui::CCalibPointsOrderDialog *ui;

    CCalibPointsOrderDialogPrivate * const d_ptr;
};

#endif // CCALIBPOINTSORDERDIALOG_H
