#ifndef CADDPATHPOINTDIALOG_H
#define CADDPATHPOINTDIALOG_H

#include <QDialog>

namespace GUI_TYPES {
struct SPathPoint;
}

namespace Ui {
class CAddPathPointDialog;
}

class CAddPathPointDialog : public QDialog
{
public:
    explicit CAddPathPointDialog(QWidget *parent, const GUI_TYPES::SPathPoint &initData);
    ~CAddPathPointDialog();

    GUI_TYPES::SPathPoint getPathPoint() const;

private:
    Ui::CAddPathPointDialog *ui;
};

#endif // CADDPATHPOINTDIALOG_H
