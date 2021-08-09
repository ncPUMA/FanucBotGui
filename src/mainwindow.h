#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class OpenGl_GraphicDriver;
class MainWindowPrivate;
class CAbstractSettingsStorage;

class CAbstractBotSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class CModelMover;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init(OpenGl_GraphicDriver &driver);
    void setSettingsStorage(CAbstractSettingsStorage &storage);
    void setBotSocket(CAbstractBotSocket &botSocket);

private slots:
    //Menu "File"
    void slImport();
    void slCalibPointsOrderDlg();
    void slTaskPointsOrderDlg();
    void slExit();

    //Menu "View"
    void slShading(bool enabled);
    void slShowCalibWidget(bool enabled);
    void slMsaa();
    void slFpsCounter(bool enabled);
    void slClearJrnl();

    //callib
    void slCallibApply();
    void slCallibCalc();

    //start/stop
    void slStart();
    void slStop();

    //For LBot
    void slUpdateBotLamps();

private:
    void configMenu();
    void configToolBar();

private:
    Ui::MainWindow *ui;
    MainWindowPrivate * const d_ptr;
};
#endif // MAINWINDOW_H
