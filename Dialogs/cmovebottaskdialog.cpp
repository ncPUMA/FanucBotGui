#include "cmovebottaskdialog.h"
#include "ui_cmovebottaskdialog.h"

#include <gp_Pnt.hxx>

CMoveBotTaskDialog::CMoveBotTaskDialog(QWidget *parent, const gp_Pnt pos) :
    QDialog(parent),
    ui(new Ui::CMoveBotTaskDialog)
{
    ui->setupUi(this);

    ui->dsbGlobalX->setValue(pos.X());
    ui->dsbGlobalY->setValue(pos.Y());
    ui->dsbGlobalZ->setValue(pos.Z());

    connect(ui->pbOk, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbCancel, &QAbstractButton::clicked, this, &QDialog::reject);
}

CMoveBotTaskDialog::~CMoveBotTaskDialog()
{
    delete ui;
}

gp_Pnt CMoveBotTaskDialog::getPos() const
{
    gp_Pnt res;
    res.SetX(ui->dsbGlobalX->value());
    res.SetY(ui->dsbGlobalY->value());
    res.SetZ(ui->dsbGlobalZ->value());
    return res;
}

gp_Pnt CMoveBotTaskDialog::getAngles() const
{
    gp_Pnt res;
    res.SetX(ui->dsbApha->value());
    res.SetY(ui->dsbBeta->value());
    res.SetZ(ui->dsbGamma->value());
    return res;
}
