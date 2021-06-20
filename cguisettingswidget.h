#ifndef CGUISETTINGSWIDGET_H
#define CGUISETTINGSWIDGET_H

#include <QWidget>

#include <gui_types.h>

namespace Ui {
class CGuiSettingsWidget;
}

class CAbstractGuiSettings;

class CGuiSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGuiSettingsWidget(QWidget *parent = nullptr);
    ~CGuiSettingsWidget();

    void initFromGuiSettings(const CAbstractGuiSettings &settings);
    void applyToGuiSettings(CAbstractGuiSettings &settings);

signals:
    void sigApplyRequest();

private:
    Ui::CGuiSettingsWidget *ui;
};

#endif // CGUISETTINGSWIDGET_H
