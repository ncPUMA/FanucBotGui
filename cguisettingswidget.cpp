#include "cguisettingswidget.h"
#include "ui_cguisettingswidget.h"

#include "cabstractguisettings.h"

enum EN_CbBotCoordTypeIndexes
{
    ENCBCTI_RIGHT = 0,
    ENCBCTI_LEFT  = 1
};

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
    //The LaserBot
    ui->dsbBotAnchorX->setValue(settings.getBotAnchorX());
    ui->dsbBotAnchorY->setValue(settings.getBotAnchorY());
    ui->dsbBotAnchorZ->setValue(settings.getBotAnchorZ());
    ui->dsbBotLaserX->setValue(settings.getBotLaserX());
    ui->dsbBotLaserY->setValue(settings.getBotLaserY());
    ui->dsbBotLaserZ->setValue(settings.getBotLaserZ());
    if (settings.getBotCoordType() == GUI_TYPES::ENCS_RIGHT)
        ui->cbBotCoordType->setCurrentIndex(ENCBCTI_RIGHT);
    else
        ui->cbBotCoordType->setCurrentIndex(ENCBCTI_LEFT);

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

    //The Grip
    ui->checkGripVis->setChecked(settings.isGripVisible());
    ui->dsbGripTrX->setValue(settings.getGripTrX());
    ui->dsbGripTrY->setValue(settings.getGripTrY());
    ui->dsbGripTrZ->setValue(settings.getGripTrZ());
    ui->dsbGripCenterX->setValue(settings.getGripCenterX());
    ui->dsbGripCenterY->setValue(settings.getGripCenterY());
    ui->dsbGripCenterZ->setValue(settings.getGripCenterZ());
    ui->dsbGripRotateX->setValue(settings.getGripRotationX());
    ui->dsbGripRotateY->setValue(settings.getGripRotationY());
    ui->dsbGripRotateZ->setValue(settings.getGripRotationZ());
    ui->dsbGripScale->setValue(settings.getGripScale());
}

void CGuiSettingsWidget::applyToGuiSettings(CAbstractGuiSettings &settings)
{
    //The LaserBot
    settings.setBotAnchorX(ui->dsbBotAnchorX->value());
    settings.setBotAnchorY(ui->dsbBotAnchorY->value());
    settings.setBotAnchorZ(ui->dsbBotAnchorZ->value());
    settings.setBotLaserX(ui->dsbBotLaserX->value());
    settings.setBotLaserY(ui->dsbBotLaserY->value());
    settings.setBotLaserZ(ui->dsbBotLaserZ->value());
    if (ui->cbBotCoordType->currentIndex() == ENCBCTI_RIGHT)
        settings.setBotCoordType(GUI_TYPES::ENCS_RIGHT);
    else
        settings.setBotCoordType(GUI_TYPES::ENCS_LEFT);

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

    //The Grip
    settings.setGripVisible(ui->checkGripVis->isChecked());
    settings.setGripTrX(ui->dsbGripTrX->value());
    settings.setGripTrY(ui->dsbGripTrY->value());
    settings.setGripTrZ(ui->dsbGripTrZ->value());
    settings.setGripCenterX(ui->dsbGripCenterX->value());
    settings.setGripCenterY(ui->dsbGripCenterY->value());
    settings.setGripCenterZ(ui->dsbGripCenterZ->value());
    settings.setGripRotationX(ui->dsbGripRotateX->value());
    settings.setGripRotationY(ui->dsbGripRotateY->value());
    settings.setGripRotationZ(ui->dsbGripRotateZ->value());
    settings.setGripScale(ui->dsbGripScale->value());
}
