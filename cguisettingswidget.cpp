#include "cguisettingswidget.h"
#include "ui_cguisettingswidget.h"

#include "cabstractguisettings.h"

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

void CGuiSettingsWidget::initFromGuiSettings(const CAbstractGuiSettings &settings)
{
    //The Part
    ui->dsbPartTrX->setValue(settings.getPartTrX());
    ui->dsbPartTrY->setValue(settings.getPartTrY());
    ui->dsbPartTrZ->setValue(settings.getPartTrZ());
    ui->dsbPartCenterX->setValue(settings.getPartCenterX());
    ui->dsbPartCenterY->setValue(settings.getPartCenterY());
    ui->dsbPartCenterZ->setValue(settings.getPartCenterZ());
    ui->dsbPartRotateX->setValue(settings.getPartRotationX());
    ui->dsbPartRotateY->setValue(settings.getPartRotationY());
    ui->dsbPartRotateZ->setValue(settings.getPartRotationZ());
    ui->dsbPartScale->setValue(settings.getPartScale());
}

void CGuiSettingsWidget::applyToGuiSettings(CAbstractGuiSettings &settings)
{
    //The Part
    settings.setPartTrX(ui->dsbPartTrX->value());
    settings.setPartTrY(ui->dsbPartTrY->value());
    settings.setPartTrZ(ui->dsbPartTrZ->value());
    settings.setPartCenterX(ui->dsbPartCenterX->value());
    settings.setPartCenterY(ui->dsbPartCenterY->value());
    settings.setPartCenterZ(ui->dsbPartCenterZ->value());
    settings.setPartRotationX(ui->dsbPartRotateX->value());
    settings.setPartRotationY(ui->dsbPartRotateY->value());
    settings.setPartRotationZ(ui->dsbPartRotateZ->value());
    settings.setPartScale(ui->dsbPartScale->value());
}
