#include "cmarkbottaskdialog.h"
#include "ui_cmarkbottaskdialog.h"

#include "../../gui_types.h"

CMarkBotTaskDialog::CMarkBotTaskDialog(QWidget *parent, const GUI_TYPES::STaskPoint &initData) :
    QDialog(parent),
    ui(new Ui::CMarkBotTaskDialog)
{
    ui->setupUi(this);

    ui->dsbGlobalX->setValue(initData.globalPos.x);
    ui->dsbGlobalY->setValue(initData.globalPos.y);
    ui->dsbGlobalZ->setValue(initData.globalPos.z);

    connect(ui->pbOk, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbCancel, &QAbstractButton::clicked, this, &QDialog::reject);
}

CMarkBotTaskDialog::~CMarkBotTaskDialog()
{
    delete ui;
}

GUI_TYPES::STaskPoint CMarkBotTaskDialog::getTaskPoint() const
{
    GUI_TYPES::STaskPoint res;
    res.taskType = GUI_TYPES::ENBTT_MARK;
    res.globalPos.x = ui->dsbGlobalX->value();
    res.globalPos.y = ui->dsbGlobalY->value();
    res.globalPos.z = ui->dsbGlobalZ->value();
    res.angle.x = ui->dsbApha->value();
    res.angle.y = ui->dsbBeta->value();
    res.angle.z = ui->dsbGamma->value();
    return res;
}
