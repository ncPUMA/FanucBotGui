#include "caddcalibpointdialog.h"
#include "ui_caddcalibpointdialog.h"

#include "../../gui_types.h"

CAddCalibPointDialog::CAddCalibPointDialog(QWidget *parent, const GUI_TYPES::SCalibPoint &initData) :
    QDialog(parent),
    ui(new Ui::CAddCalibPointDialog)
{
    ui->setupUi(this);

    ui->dsbGlobalX->setValue(initData.globalPos.x);
    ui->dsbGlobalY->setValue(initData.globalPos.y);
    ui->dsbGlobalZ->setValue(initData.globalPos.z);
    ui->dsbBotX->setValue(initData.botPos.x);
    ui->dsbBotY->setValue(initData.botPos.y);
    ui->dsbBotZ->setValue(initData.botPos.z);

    connect(ui->pbOk, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbCancel, &QAbstractButton::clicked, this, &QDialog::reject);
}

CAddCalibPointDialog::~CAddCalibPointDialog()
{
    delete ui;
}

GUI_TYPES::SCalibPoint CAddCalibPointDialog::getCalibPoint() const
{
    GUI_TYPES::SCalibPoint res;
    res.globalPos.x = ui->dsbGlobalX->value();
    res.globalPos.y = ui->dsbGlobalY->value();
    res.globalPos.z = ui->dsbGlobalZ->value();
    res.botPos.x = ui->dsbBotX->value();
    res.botPos.y = ui->dsbBotY->value();
    res.botPos.z = ui->dsbBotZ->value();
    return res;
}
