#ifndef CPATHPOINTSORDERDIALOG_H
#define CPATHPOINTSORDERDIALOG_H

#include <QDialog>

#include "../../gui_types.h"

namespace Ui {
class CPathPointsOrderDialog;
}

class CPathPointsOrderDialogPrivate;

class CPathPointsOrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CPathPointsOrderDialog(const std::vector <GUI_TYPES::SPathPoint> &pathPnts,
                                    QWidget *parent = nullptr);
    ~CPathPointsOrderDialog();

    std::vector<GUI_TYPES::SPathPoint> getPathPoints() const;

private:
    Ui::CPathPointsOrderDialog *ui;

    CPathPointsOrderDialogPrivate * const d_ptr;
};

#endif // CPATHPOINTSORDERDIALOG_H
