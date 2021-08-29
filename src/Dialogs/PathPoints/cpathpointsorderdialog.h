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
    explicit CPathPointsOrderDialog(const std::vector <GUI_TYPES::SHomePoint> &homePnts,
                                    QWidget *parent = nullptr);
    ~CPathPointsOrderDialog();

    std::vector<GUI_TYPES::SHomePoint> getHomePoints() const;

private:
    Ui::CPathPointsOrderDialog *ui;

    CPathPointsOrderDialogPrivate * const d_ptr;
};

#endif // CPATHPOINTSORDERDIALOG_H
