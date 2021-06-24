#include "caddcalibpointdialog.h"
#include "ui_caddcalibpointdialog.h"

#include <gp_Pnt.hxx>

CAddCalibPointDialog::CAddCalibPointDialog(const gp_Pnt &pos, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAddCalibPointDialog)
{
    ui->setupUi(this);

    ui->dsbGlobalX->setValue(pos.X());
    ui->dsbGlobalY->setValue(pos.Y());
    ui->dsbGlobalZ->setValue(pos.Z());
    ui->dsbBotX->setValue(pos.X());
    ui->dsbBotY->setValue(pos.Y());
    ui->dsbBotZ->setValue(pos.Z());

    connect(ui->pbOk, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbCancel, &QAbstractButton::clicked, this, &QDialog::reject);
}

CAddCalibPointDialog::~CAddCalibPointDialog()
{
    delete ui;
}

gp_Pnt CAddCalibPointDialog::getGlobalPos() const
{
    return gp_Pnt(ui->dsbGlobalX->value(),
                  ui->dsbGlobalY->value(),
                  ui->dsbGlobalZ->value());
}

gp_Pnt CAddCalibPointDialog::getBotPos() const
{
    return gp_Pnt(ui->dsbBotX->value(),
                  ui->dsbBotY->value(),
                  ui->dsbBotZ->value());
}
