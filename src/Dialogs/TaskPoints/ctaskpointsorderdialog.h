#ifndef CTASKPOINTSORDERDIALOG_H
#define CTASKPOINTSORDERDIALOG_H

#include <QDialog>

#include "../../gui_types.h"

namespace Ui {
class CTaskPointsOrderDialog;
}

class CTaskPointsOrderDialogPrivate;

class CTaskPointsOrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CTaskPointsOrderDialog(const std::vector <GUI_TYPES::STaskPoint> &taskPnts,
                                    QWidget *parent = nullptr);
    ~CTaskPointsOrderDialog();

    std::vector<GUI_TYPES::STaskPoint> getTaskPoints() const;

private:
    Ui::CTaskPointsOrderDialog *ui;

    CTaskPointsOrderDialogPrivate * const d_ptr;
};

#endif // CTASKPOINTSORDERDIALOG_H
