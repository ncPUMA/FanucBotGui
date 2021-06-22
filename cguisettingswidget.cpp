#include "cguisettingswidget.h"
#include "ui_cguisettingswidget.h"

#include "sguisettings.h"

CGuiSettingsWidget::CGuiSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CGuiSettingsWidget)
{
    ui->setupUi(this);

    connect(ui->pbApply, SIGNAL(clicked(bool)), this, SIGNAL(sigApplyRequest()));
}

CGuiSettingsWidget::~CGuiSettingsWidget()
{
    delete ui;
}

void CGuiSettingsWidget::initFromGuiSettings(const SGuiSettings &settings)
{
    //The Part
    ui->dsbPartTrX->setValue(settings.partTrX);
    ui->dsbPartTrY->setValue(settings.partTrY);
    ui->dsbPartTrZ->setValue(settings.partTrZ);
    ui->dsbPartCenterX->setValue(settings.partCenterX);
    ui->dsbPartCenterY->setValue(settings.partCenterY);
    ui->dsbPartCenterZ->setValue(settings.partCenterZ);
    ui->dsbPartRotateX->setValue(settings.partRotationX);
    ui->dsbPartRotateY->setValue(settings.partRotationY);
    ui->dsbPartRotateZ->setValue(settings.partRotationZ);
    ui->dsbPartScale->setValue(settings.partScale);
}

SGuiSettings CGuiSettingsWidget::getChangedSettings() const
{
    SGuiSettings settings;
    settings.partTrX       = ui->dsbPartTrX->value();
    settings.partTrY       = ui->dsbPartTrY->value();
    settings.partTrZ       = ui->dsbPartTrZ->value();
    settings.partCenterX   = ui->dsbPartCenterX->value();
    settings.partCenterY   = ui->dsbPartCenterY->value();
    settings.partCenterZ   = ui->dsbPartCenterZ->value();
    settings.partRotationX = ui->dsbPartRotateX->value();
    settings.partRotationY = ui->dsbPartRotateY->value();
    settings.partRotationZ = ui->dsbPartRotateZ->value();
    settings.partScale     = ui->dsbPartScale->value();
    return settings;
}
