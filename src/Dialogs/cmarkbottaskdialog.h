#ifndef CMARKBOTTASKDIALOG_H
#define CMARKBOTTASKDIALOG_H

#include <QDialog>

namespace GUI_TYPES {
struct STaskPoint;
}

namespace Ui {
class CMarkBotTaskDialog;
}

class CMarkBotTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CMarkBotTaskDialog(QWidget *parent, const GUI_TYPES::STaskPoint &initData);
    ~CMarkBotTaskDialog();

    GUI_TYPES::STaskPoint getTaskPoint() const;

private:
    Ui::CMarkBotTaskDialog *ui;
};

#endif // CMARKBOTTASKDIALOG_H
