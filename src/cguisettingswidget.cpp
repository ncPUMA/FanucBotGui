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

void CGuiSettingsWidget::initFromGuiSettings(const GUI_TYPES::SGuiSettings &settings)
{
    //The Part
    ui->dsbPartTrX->setValue    (settings.partTrX      );
    ui->dsbPartTrY->setValue    (settings.partTrY      );
    ui->dsbPartTrZ->setValue    (settings.partTrZ      );
    ui->dsbPartCenterX->setValue(settings.partCenterX  );
    ui->dsbPartCenterY->setValue(settings.partCenterY  );
    ui->dsbPartCenterZ->setValue(settings.partCenterZ  );
    ui->dsbPartRotateX->setValue(settings.partRotationX);
    ui->dsbPartRotateY->setValue(settings.partRotationY);
    ui->dsbPartRotateZ->setValue(settings.partRotationZ);
    ui->dsbPartScale->setValue  (settings.partScale    );
    //The Desk
    ui->dsbDeskTrX->setValue    (settings.deskTrX      );
    ui->dsbDeskTrY->setValue    (settings.deskTrY      );
    ui->dsbDeskTrZ->setValue    (settings.deskTrZ      );
    ui->dsbDeskCenterX->setValue(settings.deskCenterX  );
    ui->dsbDeskCenterY->setValue(settings.deskCenterY  );
    ui->dsbDeskCenterZ->setValue(settings.deskCenterZ  );
    ui->dsbDeskRotateX->setValue(settings.deskRotationX);
    ui->dsbDeskRotateY->setValue(settings.deskRotationY);
    ui->dsbDeskRotateZ->setValue(settings.deskRotationZ);
    ui->dsbDeskScale->setValue  (settings.deskScale    );
    //The Laser Head
    ui->dsbLHeadTrX->setValue    (settings.lheadTrX      );
    ui->dsbLHeadTrY->setValue    (settings.lheadTrY      );
    ui->dsbLHeadTrZ->setValue    (settings.lheadTrZ      );
    ui->dsbLHeadCenterX->setValue(settings.lheadCenterX  );
    ui->dsbLHeadCenterY->setValue(settings.lheadCenterY  );
    ui->dsbLHeadCenterZ->setValue(settings.lheadCenterZ  );
    ui->dsbLHeadRotateX->setValue(settings.lheadRotationX);
    ui->dsbLHeadRotateY->setValue(settings.lheadRotationY);
    ui->dsbLHeadRotateZ->setValue(settings.lheadRotationZ);
    ui->dsbLHeadScale->setValue  (settings.lheadScale    );
    //The Grip
    ui->dsbGripTrX->setValue    (settings.gripTrX      );
    ui->dsbGripTrY->setValue    (settings.gripTrY      );
    ui->dsbGripTrZ->setValue    (settings.gripTrZ      );
    ui->dsbGripCenterX->setValue(settings.gripCenterX  );
    ui->dsbGripCenterY->setValue(settings.gripCenterY  );
    ui->dsbGripCenterZ->setValue(settings.gripCenterZ  );
    ui->dsbGripRotateX->setValue(settings.gripRotationX);
    ui->dsbGripRotateY->setValue(settings.gripRotationY);
    ui->dsbGripRotateZ->setValue(settings.gripRotationZ);
    ui->dsbGripScale->setValue  (settings.gripScale    );
}

GUI_TYPES::SGuiSettings CGuiSettingsWidget::getChangedSettings() const
{
    GUI_TYPES::SGuiSettings settings;
    //The Part
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
    //The Desk
    settings.deskTrX       = ui->dsbDeskTrX->value();
    settings.deskTrY       = ui->dsbDeskTrY->value();
    settings.deskTrZ       = ui->dsbDeskTrZ->value();
    settings.deskCenterX   = ui->dsbDeskCenterX->value();
    settings.deskCenterY   = ui->dsbDeskCenterY->value();
    settings.deskCenterZ   = ui->dsbDeskCenterZ->value();
    settings.deskRotationX = ui->dsbDeskRotateX->value();
    settings.deskRotationY = ui->dsbDeskRotateY->value();
    settings.deskRotationZ = ui->dsbDeskRotateZ->value();
    settings.deskScale     = ui->dsbDeskScale->value();
    //The Laser Head
    settings.lheadTrX       = ui->dsbLHeadTrX->value();
    settings.lheadTrY       = ui->dsbLHeadTrY->value();
    settings.lheadTrZ       = ui->dsbLHeadTrZ->value();
    settings.lheadCenterX   = ui->dsbLHeadCenterX->value();
    settings.lheadCenterY   = ui->dsbLHeadCenterY->value();
    settings.lheadCenterZ   = ui->dsbLHeadCenterZ->value();
    settings.lheadRotationX = ui->dsbLHeadRotateX->value();
    settings.lheadRotationY = ui->dsbLHeadRotateY->value();
    settings.lheadRotationZ = ui->dsbLHeadRotateZ->value();
    settings.lheadScale     = ui->dsbLHeadScale->value();
    //The Grip
    settings.gripTrX       = ui->dsbGripTrX->value();
    settings.gripTrY       = ui->dsbGripTrY->value();
    settings.gripTrZ       = ui->dsbGripTrZ->value();
    settings.gripCenterX   = ui->dsbGripCenterX->value();
    settings.gripCenterY   = ui->dsbGripCenterY->value();
    settings.gripCenterZ   = ui->dsbGripCenterZ->value();
    settings.gripRotationX = ui->dsbGripRotateX->value();
    settings.gripRotationY = ui->dsbGripRotateY->value();
    settings.gripRotationZ = ui->dsbGripRotateZ->value();
    settings.gripScale     = ui->dsbGripScale->value();
    return settings;
}
