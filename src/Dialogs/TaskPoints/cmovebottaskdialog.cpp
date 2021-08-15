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
    ui->dsbApha->setValue(initData.angle.x);
    ui->dsbBeta->setValue(initData.angle.y);
    ui->dsbGamma->setValue(initData.angle.z);
    ui->dsbNormalX->setValue(initData.normal.x);
    ui->dsbNormalY->setValue(initData.normal.y);
    ui->dsbNormalZ->setValue(initData.normal.z);
    ui->checkZSimmetry->setChecked(initData.zSimmetry);

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
    res.normal.x = ui->dsbNormalX->value();
    res.normal.y = ui->dsbNormalY->value();
    res.normal.z = ui->dsbNormalZ->value();
    res.zSimmetry = ui->checkZSimmetry->isChecked();
    return res;
}
