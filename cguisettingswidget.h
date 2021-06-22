#ifndef CGUISETTINGSWIDGET_H
#define CGUISETTINGSWIDGET_H

#include <QWidget>

#include <gui_types.h>

namespace Ui {
class CGuiSettingsWidget;
}

class SGuiSettings;

class CGuiSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGuiSettingsWidget(QWidget *parent = nullptr);
    ~CGuiSettingsWidget();

    void initFromGuiSettings(const SGuiSettings &settings);
    SGuiSettings getChangedSettings() const;

signals:
    void sigApplyRequest();

private:
    Ui::CGuiSettingsWidget *ui;
};

#endif // CGUISETTINGSWIDGET_H
