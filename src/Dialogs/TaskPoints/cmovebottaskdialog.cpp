#include "cmovebottaskdialog.h"
#include "ui_cmovebottaskdialog.h"

#include "../../gui_types.h"

CMoveBotTaskDialog::CMoveBotTaskDialog(QWidget *parent, const GUI_TYPES::STaskPoint &initData) :
    QDialog(parent),
    ui(new Ui::CMoveBotTaskDialog)
{
    ui->setupUi(this);

    ui->dsbGlobalX->setValue(initData.globalPos.x);
    ui->dsbGlobalY->setValue(initData.globalPos.y);
    ui->dsbGlobalZ->setValue(initData.globalPos.z);

    connect(ui->pbOk, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbCancel, &QAbstractButton::clicked, this, &QDialog::reject);
}

CMoveBotTaskDialog::~CMoveBotTaskDialog()
{
    delete ui;
}

GUI_TYPES::STaskPoint CMoveBotTaskDialog::getTaskPoint() const
{
    GUI_TYPES::STaskPoint res;
    res.taskType = GUI_TYPES::ENBTT_MOVE;
    res.globalPos.x = ui->dsbGlobalX->value();
    res.globalPos.y = ui->dsbGlobalY->value();
    res.globalPos.z = ui->dsbGlobalZ->value();
    res.angle.x = ui->dsbApha->value();
    res.angle.y = ui->dsbBeta->value();
    res.angle.z = ui->dsbGamma->value();
    return res;
}
