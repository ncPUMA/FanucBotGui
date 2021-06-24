#include "cdrillbottaskdialog.h"
#include "ui_cdrillbottaskdialog.h"

#include <gp_Pnt.hxx>

CDrillBotTaskDialog::CDrillBotTaskDialog(QWidget *parent, const gp_Pnt pos) :
    QDialog(parent),
    ui(new Ui::CDrillBotTaskDialog)
{
    ui->setupUi(this);

    ui->dsbGlobalX->setValue(pos.X());
    ui->dsbGlobalY->setValue(pos.Y());
    ui->dsbGlobalZ->setValue(pos.Z());

    connect(ui->pbOk, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbCancel, &QAbstractButton::clicked, this, &QDialog::reject);
}

CDrillBotTaskDialog::~CDrillBotTaskDialog()
{
    delete ui;
}

gp_Pnt CDrillBotTaskDialog::getPos() const
{
    gp_Pnt res;
    res.SetX(ui->dsbGlobalX->value());
    res.SetY(ui->dsbGlobalY->value());
    res.SetZ(ui->dsbGlobalZ->value());
    return res;
}

gp_Pnt CDrillBotTaskDialog::getAngles() const
{
    gp_Pnt res;
    res.SetX(ui->dsbApha->value());
    res.SetY(ui->dsbBeta->value());
    res.SetZ(ui->dsbGamma->value());
    return res;
}
