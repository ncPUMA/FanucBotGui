#ifndef CGUISETTINGSWIDGET_H
#define CGUISETTINGSWIDGET_H

#include <QWidget>

#include <gui_types.h>

namespace Ui {
class CGuiSettingsWidget;
}

namespace GUI_TYPES {
class SGuiSettings;
}

class CGuiSettingsWidgetPrivate;

class CGuiSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGuiSettingsWidget(QWidget *parent = nullptr);
    ~CGuiSettingsWidget();

    void initFromGuiSettings(const GUI_TYPES::SGuiSettings &settings);
    GUI_TYPES::SGuiSettings getChangedSettings() const;

signals:
    void sigApplyRequest();
    void sigCalcCalibration();

private slots:
    void slShowMoreOrLess();

private:
    Ui::CGuiSettingsWidget *ui;
    CGuiSettingsWidgetPrivate * const d_ptr;
};

#endif // CGUISETTINGSWIDGET_H
