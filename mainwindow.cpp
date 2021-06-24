#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QLabel>

#include "cabstractsettingsstorage.h"
#include "ModelLoader/cmodelloaderfactorymethod.h"
#include "ModelLoader/csteploader.h"

static constexpr int MAX_JRNL_ROW_COUNT = 15000;

class MainWindowPrivate
{
    friend class MainWindow;
    friend class CModelMover;

private:
    MainWindowPrivate() :
        settingsStorage(&emptySettingsStorage)
    { }

    void setMSAA(const GUI_TYPES::TMSAA value, CMainViewport &view) {
        for(auto pair : mapMsaa) {
            pair.second->blockSignals(true);
            pair.second->setChecked(pair.first == value);
            pair.second->blockSignals(false);
        }
        view.setMSAA(value);
    }

private:
    std::map <GUI_TYPES::TMSAA, QAction *> mapMsaa;

    CEmptySettingsStorage emptySettingsStorage;
    CAbstractSettingsStorage *settingsStorage;
};



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    d_ptr(new MainWindowPrivate())
{
    ui->setupUi(this);

    configMenu();
    configToolBar();

    //Callib
    connect(ui->wSettings, SIGNAL(sigApplyRequest()), SLOT(slCallibApply()));

    ui->teJrnl->document()->setMaximumBlockCount(MAX_JRNL_ROW_COUNT);
}

MainWindow::~MainWindow()
{
    delete d_ptr;
    delete ui;
}

void MainWindow::init(OpenGl_GraphicDriver &driver)
{
    ui->mainView->init(driver);
    ui->mainView->setStatsVisible(ui->actionFPS->isChecked());

    //load default mainModel
    QFile defaultModelFile(":/Models/Data/Models/turbine_blade.stp");
    if (defaultModelFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const QByteArray stepData = defaultModelFile.readAll();
        CStepLoader loader;
        const TopoDS_Shape mdl = loader.loadFromBinaryData(stepData.constData(),
                                                           static_cast <size_t> (stepData.size()));
        ui->mainView->setMainModel(mdl);
    }

    //load gripModel
    QFile gripFile(":/Models/Data/Models/MHZ2_16D_grip.stp");
    if (gripFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const QByteArray stepData = gripFile.readAll();
        CStepLoader loader;
        const TopoDS_Shape mdl = loader.loadFromBinaryData(stepData.constData(),
                                                           static_cast <size_t> (stepData.size()));
        ui->mainView->setGripModel(mdl);
    }

    ui->mainView->setUserAction(GUI_TYPES::ENUA_ADD_TASK);
}

void MainWindow::setSettingsStorage(CAbstractSettingsStorage &storage)
{
    d_ptr->settingsStorage = &storage;
    const SGuiSettings settings = storage.loadGuiSettings();
    for(auto pair : d_ptr->mapMsaa) {
        pair.second->blockSignals(true);
        pair.second->setChecked(pair.first == settings.msaa);
        pair.second->blockSignals(false);
    }
    ui->wSettings->initFromGuiSettings(settings);
    ui->mainView->setGuiSettings(settings);
    ui->mainView->updateModelsDefaultPosition(ui->actionShading->isChecked());
    ui->mainView->fitInView();
}

void MainWindow::slImport()
{
    CModelLoaderFactoryMethod factory;
    QString selectedFilter;
    const QString fName =
            QFileDialog::getOpenFileName(this,
                                         tr("Выбор файла"),
                                         QString(),
                                         factory.supportedFilters(),
                                         &selectedFilter);
    if (!fName.isNull())
    {
        CAbstractModelLoader &loader = factory.loader(selectedFilter);
        const TopoDS_Shape shape = loader.load(fName.toStdString().c_str());
        ui->mainView->setMainModel(shape);
        ui->mainView->updateModelsDefaultPosition(ui->actionShading->isChecked());
        if (!shape.IsNull())
            ui->mainView->fitInView();
        else
            QMessageBox::critical(this,
                                  tr("Ошибка загрузки файла"),
                                  tr("Ошибка загрузки файла"));
    }
}

void MainWindow::slExit()
{
    close();
}

void MainWindow::slShading(bool enabled)
{
    ui->mainView->updateModelsDefaultPosition(enabled);
}

void MainWindow::slShowCalibWidget(bool enabled)
{
    ui->dockSettings->setVisible(enabled);
    ui->mainView->setUserAction(enabled ? GUI_TYPES::ENUA_CALIBRATION : GUI_TYPES::ENUA_ADD_TASK);
    ui->mainView->updateModelsDefaultPosition(ui->actionShading->isChecked());
}

void MainWindow::slMsaa()
{
    GUI_TYPES::TMSAA msaa = GUI_TYPES::ENMSAA_OFF;
    for(auto pair : d_ptr->mapMsaa)
    {
        if (pair.second == sender())
        {
            msaa = pair.first;
            break;
        }
    }
    for(auto pair : d_ptr->mapMsaa) {
        pair.second->blockSignals(true);
        pair.second->setChecked(pair.first == msaa);
        pair.second->blockSignals(false);
    }
    ui->mainView->setMSAA(msaa);
    d_ptr->settingsStorage->saveGuiSettings(ui->mainView->getGuiSettings());
}

void MainWindow::slFpsCounter(bool enabled)
{
    ui->mainView->setStatsVisible(enabled);
}

void MainWindow::slClearJrnl()
{
    ui->teJrnl->clear();
}

void MainWindow::slCallibApply()
{
    SGuiSettings settings = ui->wSettings->getChangedSettings();
    settings.msaa = ui->mainView->getMSAA();
    d_ptr->settingsStorage->saveGuiSettings(settings);
    ui->mainView->setGuiSettings(settings);
    ui->mainView->updateModelsDefaultPosition(ui->actionShading->isChecked());
}

void MainWindow::configMenu()
{
    //Menu "File"
    connect(ui->actionImport, SIGNAL(triggered(bool)), SLOT(slImport()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), SLOT(slExit()));

    //Menu "View"
    connect(ui->actionShading, SIGNAL(toggled(bool)), SLOT(slShading(bool)));
    ui->dockSettings->setVisible(false);
    connect(ui->actionCalib, SIGNAL(toggled(bool)), SLOT(slShowCalibWidget(bool)));
    //MSAA
    d_ptr->mapMsaa = std::map <GUI_TYPES::TMSAA, QAction *> {
        { GUI_TYPES::ENMSAA_OFF, ui->actionMSAA_Off },
        { GUI_TYPES::ENMSAA_2  , ui->actionMSAA_2X  },
        { GUI_TYPES::ENMSAA_4  , ui->actionMSAA_4X  },
        { GUI_TYPES::ENMSAA_8  , ui->actionMSAA_8X  }
    };
    for(auto pair : d_ptr->mapMsaa)
        connect(pair.second, SIGNAL(toggled(bool)), SLOT(slMsaa()));
    //FPS
    connect(ui->actionFPS, SIGNAL(toggled(bool)), SLOT(slFpsCounter(bool)));

    //teJrnl
    connect(ui->actionClearJrnl, SIGNAL(triggered(bool)), SLOT(slClearJrnl()));
}

void MainWindow::configToolBar()
{
    //ToolBar
    ui->toolBar->addAction(QIcon(":/icons/Data/Icons/open.png"),
                           tr("Импорт..."),
                           ui->actionImport,
                           SLOT(trigger()));
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(QIcon(":/icons/Data/Icons/shading.png"),
                           tr("Shading"),
                           ui->actionShading,
                           SLOT(toggle()));
    ui->toolBar->addAction(QIcon(":/icons/Data/Icons/fps-counter.png"),
                           tr("Счетчик FPS"),
                           ui->actionFPS,
                           SLOT(toggle()));
}

