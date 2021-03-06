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
    ui->checkCalib->setChecked(initData.bNeedCalib);
    ui->dsbApha->setValue(initData.angle.x);
    ui->dsbBeta->setValue(initData.angle.y);
    ui->dsbGamma->setValue(initData.angle.z);
    ui->dsbNormalX->setValue(initData.normal.x);
    ui->dsbNormalY->setValue(initData.normal.y);
    ui->dsbNormalZ->setValue(initData.normal.z);
    ui->checkZSimmetry->setChecked(initData.zSimmetry);
    ui->dsbDelay->setValue(initData.delay);
    ui->checkHomePnt->setChecked(initData.bUseHomePnt);

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
    res.bNeedCalib = ui->checkCalib->isChecked();
    res.angle.x = ui->dsbApha->value();
    res.angle.y = ui->dsbBeta->value();
    res.angle.z = ui->dsbGamma->value();
    res.normal.x = ui->dsbNormalX->value();
    res.normal.y = ui->dsbNormalY->value();
    res.normal.z = ui->dsbNormalZ->value();
    res.zSimmetry = ui->checkZSimmetry->isChecked();
    res.delay = ui->dsbDelay->value();
    res.bUseHomePnt = ui->checkHomePnt->isChecked();
    return res;
}
