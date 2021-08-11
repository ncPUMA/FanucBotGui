#ifndef CMOVEBOTTASKDIALOG_H
#define CMOVEBOTTASKDIALOG_H

#include <QDialog>

namespace GUI_TYPES {
struct STaskPoint;
}

namespace Ui {
class CMoveBotTaskDialog;
}

class CMoveBotTaskDialog : public QDialog
{
public:
    explicit CMoveBotTaskDialog(QWidget *parent, const GUI_TYPES::STaskPoint &initData);
    ~CMoveBotTaskDialog();

    GUI_TYPES::STaskPoint getTaskPoint() const;

private:
    Ui::CMoveBotTaskDialog *ui;
};

#endif // CMOVEBOTTASKDIALOG_H
