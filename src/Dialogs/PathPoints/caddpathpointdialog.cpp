#include "caddpathpointdialog.h"
#include "ui_caddpathpointdialog.h"

#include "../../gui_types.h"

CAddPathPointDialog::CAddPathPointDialog(QWidget *parent, const GUI_TYPES::SPathPoint &initData) :
    QDialog(parent),
    ui(new Ui::CAddPathPointDialog)
{
    ui->setupUi(this);

    ui->dsbGlobalX->setValue(initData.globalPos.x);
    ui->dsbGlobalY->setValue(initData.globalPos.y);
    ui->dsbGlobalZ->setValue(initData.globalPos.z);

    connect(ui->pbOk, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbCancel, &QAbstractButton::clicked, this, &QDialog::reject);
}

CAddPathPointDialog::~CAddPathPointDialog()
{
    delete ui;
}

GUI_TYPES::SPathPoint CAddPathPointDialog::getPathPoint() const
{
    GUI_TYPES::SPathPoint res;
    res.globalPos.x = ui->dsbGlobalX->value();
    res.globalPos.y = ui->dsbGlobalY->value();
    res.globalPos.z = ui->dsbGlobalZ->value();
    res.angle.x = ui->dsbApha->value();
    res.angle.y = ui->dsbBeta->value();
    res.angle.z = ui->dsbGamma->value();
    return res;
}
