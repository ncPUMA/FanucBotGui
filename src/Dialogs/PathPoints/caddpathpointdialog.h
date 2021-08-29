#ifndef CADDPATHPOINTDIALOG_H
#define CADDPATHPOINTDIALOG_H

#include <QDialog>

namespace GUI_TYPES {
struct SHomePoint;
}

namespace Ui {
class CAddPathPointDialog;
}

class CAddPathPointDialog : public QDialog
{
public:
    explicit CAddPathPointDialog(QWidget *parent, const GUI_TYPES::SHomePoint &initData);
    ~CAddPathPointDialog();

    GUI_TYPES::SHomePoint getHomePoint() const;

private:
    Ui::CAddPathPointDialog *ui;
};

#endif // CADDPATHPOINTDIALOG_H
