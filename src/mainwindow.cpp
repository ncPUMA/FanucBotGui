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

#include "BotSocket/cabstractui.h"

static constexpr int MAX_JRNL_ROW_COUNT = 15000;

class CUiIface : public CAbstractUi
{
    friend class MainWindow;

public:
    CUiIface() :
        stateLamp(new QLabel()),
        attachLamp(new QLabel()),
        viewport(nullptr),
        jrnl(nullptr) { }

    void initToolBar(QToolBar *tBar) {
        iconSize = tBar->iconSize();
        const QPixmap red =
                QPixmap(":/Lamps/Data/Lamps/red.png").scaled(iconSize,
                                                             Qt::IgnoreAspectRatio,
                                                             Qt::SmoothTransformation);
        QFont fnt = tBar->font();
        fnt.setPointSize(18);

        QLabel * const txtState = new QLabel(MainWindow::tr("Соединение: "), tBar);
        txtState->setFont(fnt);
        txtState->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tBar->addWidget(txtState);
        stateLamp->setParent(tBar);
        stateLamp->setPixmap(red);
        tBar->addWidget(stateLamp);

        QLabel * const txtAttach = new QLabel(MainWindow::tr(" Захват: "), tBar);
        txtAttach->setFont(fnt);
        txtAttach->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tBar->addWidget(txtAttach);
        attachLamp->setParent(tBar);
        attachLamp->setPixmap(red);
        tBar->addWidget(attachLamp);
    }

protected:
    void socketStateChanged(const BotSocket::TBotState state) final {
        static const QPixmap red =
                QPixmap(":/Lamps/Data/Lamps/red.png").scaled(iconSize,
                                                             Qt::IgnoreAspectRatio,
                                                             Qt::SmoothTransformation);
        static const QPixmap green =
                QPixmap(":/Lamps/Data/Lamps/green.png").scaled(iconSize,
                                                             Qt::IgnoreAspectRatio,
                                                             Qt::SmoothTransformation);
        switch(state) {
            using namespace BotSocket;
            case ENBS_FALL:
                stateLamp->setPixmap(red);
                stateLamp->setToolTip(MainWindow::tr("Авария"));
                attachLamp->setPixmap(red);
                attachLamp->setToolTip(MainWindow::tr("Нет данных"));
                break;
            case ENBS_NOT_ATTACHED:
                stateLamp->setPixmap(green);
                stateLamp->setToolTip(MainWindow::tr("ОК"));
                attachLamp->setPixmap(red);
                attachLamp->setToolTip(MainWindow::tr("Нет захвата"));
                break;
            case ENBS_ATTACHED:
                stateLamp->setPixmap(green);
                stateLamp->setToolTip(MainWindow::tr("ОК"));
                attachLamp->setPixmap(green);
                attachLamp->setToolTip(MainWindow::tr("Захват"));
                break;
            default:
                break;
        }
    }

    void laserHeadPositionChanged(const BotSocket::SBotPosition &pos) final {
        const QString jrnlTxt = MainWindow::tr(" Lsr: %1\t-->\tx: %2 y: %3 z: %4 "
                                               "α: %5 β: %6 γ: %7")
                .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                .arg(pos.globalPos.x     , 11, 'f', 6, QChar('0'))
                .arg(pos.globalPos.y     , 11, 'f', 6, QChar('0'))
                .arg(pos.globalPos.z     , 11, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.x, 11, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.y, 11, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.z, 11, 'f', 6, QChar('0'));
        jrnl->append(jrnlTxt);
        viewport->moveLsrhead(pos);
        shapeTransformChaged(BotSocket::ENST_LSRHEAD);
    }

    void gripPositionChanged(const BotSocket::SBotPosition &pos) final {
        const QString jrnlTxt = MainWindow::tr("Grip: %1\t-->\tx: %2 y: %3 z: %4 "
                                               "α: %5 β: %6 γ: %7")
                .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                .arg(pos.globalPos.x     , 11, 'f', 6, QChar('0'))
                .arg(pos.globalPos.y     , 11, 'f', 6, QChar('0'))
                .arg(pos.globalPos.z     , 11, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.x, 11, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.y, 11, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.z, 11, 'f', 6, QChar('0'));
        jrnl->append(jrnlTxt);
        viewport->moveGrip(pos);
        shapeTransformChaged(BotSocket::ENST_GRIP);
        if (viewport->getBotState() == BotSocket::ENBS_ATTACHED)
            shapeTransformChaged(BotSocket::ENST_PART);
    }

    GUI_TYPES::EN_UiStates getUiState() const final {
        return viewport->getUiState();
    }

    const TopoDS_Shape& getShape(const BotSocket::EN_ShapeType shType) const final {
        using namespace BotSocket;
        switch(shType) {
            case ENST_DESK   : return viewport->getDeskShape();
            case ENST_PART   : return viewport->getPartShape();
            case ENST_LSRHEAD: return viewport->getLsrheadShape();
            case ENST_GRIP   : return viewport->getGripShape();
            default: break;
        }
        static const TopoDS_Shape sh;
        return sh;
    }

    std::vector <GUI_TYPES::SCalibPoint> getCalibPoints() const final {
        return viewport->getCallibrationPoints();
    }

    std::vector <GUI_TYPES::STaskPoint> getTaskPoints() const {
        return viewport->getTaskPoints();
    }

private:
    QSize iconSize;
    QLabel * const stateLamp, * const attachLamp;

    CMainViewport *viewport;
    QTextEdit *jrnl;
};



class MainWindowPrivate
{
    friend class MainWindow;

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

    CUiIface uiIface;
};



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    d_ptr(new MainWindowPrivate())
{
    ui->setupUi(this);

    d_ptr->uiIface.viewport = ui->mainView;
    d_ptr->uiIface.jrnl = ui->teJrnl;

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

inline static TopoDS_Shape loadShape(const char *fName) {
    TopoDS_Shape result;
    QFile modelFile(fName);
    if (modelFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QByteArray stepData = modelFile.readAll();
        CStepLoader loader;
        result = loader.loadFromBinaryData(stepData.constData(),
                                           static_cast <size_t> (stepData.size()));
    }
    return result;
}

void MainWindow::init(OpenGl_GraphicDriver &driver)
{
    ui->mainView->init(driver);
    ui->mainView->setStatsVisible(ui->actionFPS->isChecked());

    //load default models
    ui->mainView->setPartModel(loadShape(":/Models/Data/Models/turbine_blade.stp"));
    ui->mainView->setDeskModel(loadShape(":/Models/Data/Models/WTTGA-001 - Configurable Table.stp"));
    ui->mainView->setLsrheadModel(loadShape(":/Models/Data/Models/Neje tool 30W Laser Module.stp"));
    ui->mainView->setGripModel(loadShape(":/Models/Data/Models/LDLSR30w.STEP"));

    ui->mainView->setShading(true);
    ui->mainView->setUiState(GUI_TYPES::ENUS_TASK_EDITING);
}

void MainWindow::setSettingsStorage(CAbstractSettingsStorage &storage)
{
    d_ptr->settingsStorage = &storage;
    const GUI_TYPES::SGuiSettings settings = storage.loadGuiSettings();
    for(auto pair : d_ptr->mapMsaa) {
        pair.second->blockSignals(true);
        pair.second->setChecked(pair.first == settings.msaa);
        pair.second->blockSignals(false);
    }
    ui->wSettings->initFromGuiSettings(settings);
    ui->mainView->setGuiSettings(settings);
    ui->mainView->fitInView();
}

void MainWindow::setBotSocket(CAbstractBotSocket &botSocket)
{
    using namespace BotSocket;

    d_ptr->uiIface.setBotSocket(botSocket);
    std::map <BotSocket::EN_ShapeType, TopoDS_Shape> shapes;
    shapes[ENST_DESK]    = ui->mainView->getDeskShape();
    shapes[ENST_PART]    = ui->mainView->getPartShape();
    shapes[ENST_LSRHEAD] = ui->mainView->getLsrheadShape();
    shapes[ENST_GRIP]    = ui->mainView->getGripShape();
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
        ui->mainView->setPartModel(shape);
        d_ptr->uiIface.shapeTransformChaged(BotSocket::ENST_PART);
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
    ui->mainView->setShading(enabled);
}

void MainWindow::slShowCalibWidget(bool enabled)
{
    ui->dockSettings->setVisible(enabled);
    const GUI_TYPES::EN_UiStates state = enabled
            ? GUI_TYPES::ENUS_CALIBRATION
            : GUI_TYPES::ENUS_TASK_EDITING;
    ui->mainView->setUiState(state);
    d_ptr->uiIface.uiStateChanged(state);
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
    GUI_TYPES::SGuiSettings settings = ui->wSettings->getChangedSettings();
    settings.msaa = ui->mainView->getMSAA();
    d_ptr->settingsStorage->saveGuiSettings(settings);
    ui->mainView->setGuiSettings(settings);
    d_ptr->uiIface.shapeTransformChaged(BotSocket::ENST_DESK   );
    d_ptr->uiIface.shapeTransformChaged(BotSocket::ENST_LSRHEAD);
    d_ptr->uiIface.shapeTransformChaged(BotSocket::ENST_PART   );
    d_ptr->uiIface.shapeTransformChaged(BotSocket::ENST_GRIP   );
}

void MainWindow::slStart()
{
    ui->mainView->setUiState(GUI_TYPES::ENUS_BOT_WORKED);
    d_ptr->uiIface.uiStateChanged(GUI_TYPES::ENUS_BOT_WORKED);
}

void MainWindow::slStop()
{
    const GUI_TYPES::EN_UiStates state = ui->actionCalib->isChecked()
            ? GUI_TYPES::ENUS_CALIBRATION
            : GUI_TYPES::ENUS_TASK_EDITING;
    ui->mainView->setUiState(state);
    d_ptr->uiIface.uiStateChanged(state);
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

    //Stretch
    QLabel * const strech = new QLabel(" ", ui->toolBar);
    strech->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->toolBar->addWidget(strech);
    ui->toolBar->addAction(QIcon(":/icons/Data/Icons/play.png"),
                           tr("Старт"),
                           this,
                           SLOT(slStart()));
    ui->toolBar->addAction(QIcon(":/icons/Data/Icons/stop.png"),
                           tr("Стоп"),
                           this,
                           SLOT(slStop()));

    //State and attach
    d_ptr->uiIface.initToolBar(ui->toolBar);
}

