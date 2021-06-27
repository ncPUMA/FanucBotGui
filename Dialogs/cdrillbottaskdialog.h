#ifndef CDRILLBOTTASKDIALOG_H
#define CDRILLBOTTASKDIALOG_H

#include <QDialog>

namespace GUI_TYPES {
struct STaskPoint;
}

namespace Ui {
class CDrillBotTaskDialog;
}

class CDrillBotTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CDrillBotTaskDialog(QWidget *parent, const GUI_TYPES::STaskPoint &initData);
    ~CDrillBotTaskDialog();

    GUI_TYPES::STaskPoint getTaskPoint() const;

private:
    Ui::CDrillBotTaskDialog *ui;
};

#endif // CDRILLBOTTASKDIALOG_H
