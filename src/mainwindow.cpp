#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QLabel>
#include <QTimer>

#include "cabstractsettingsstorage.h"
#include "ModelLoader/cmodelloaderfactorymethod.h"
#include "ModelLoader/csteploader.h"

#include "BotSocket/cabstractui.h"

#include "Dialogs/CalibPoints/ccalibpointsorderdialog.h"
#include "Dialogs/TaskPoints/ctaskpointsorderdialog.h"
#include "Dialogs/PathPoints/cpathpointsorderdialog.h"

#include "cjsonfilepointssaver.h"

static constexpr int MAX_JRNL_ROW_COUNT = 15000;
static const int STATE_LAMP_UPDATE_INTERVAL = 200;

class CUiIface : public CAbstractUi, public CAbstractMainViewportSubscriber
{
    friend class MainWindow;

public:
    CUiIface() :
        viewport(nullptr),
        jrnl(nullptr) { }

protected:
    void prepareComplete(const BotSocket::EN_PrepareResult) {

    }

    void tasksComplete(const BotSocket::EN_WorkResult result) {
        if(result == BotSocket::ENWR_OK)
            QMessageBox::information(viewport,
                                  QObject::tr("Задание завершено"),
                                  QObject::tr("Задание завершено"));
        else
            QMessageBox::warning(viewport,
                                  QObject::tr("Ошибка выполнения задания"),
                                  QObject::tr("Ошибка выполнения задания"));

        viewport->setUiState(GUI_TYPES::ENUS_TASK_EDITING);
    }

    void socketStateChanged(const BotSocket::EN_BotState state) final {
        viewport->setBotState(state);
    }

    void laserHeadPositionChanged(const BotSocket::SBotPosition &pos) final {
        const QString jrnlTxt = MainWindow::tr(" Lsr: %1\t-->\tx: %2 y: %3 z: %4 "
                                               "α: %5 β: %6 γ: %7")
                .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                .arg(pos.globalPos.x     , 12, 'f', 6, QChar('0'))
                .arg(pos.globalPos.y     , 12, 'f', 6, QChar('0'))
                .arg(pos.globalPos.z     , 12, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.x, 12, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.y, 12, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.z, 12, 'f', 6, QChar('0'));
        if(!usrText.isEmpty()) {
            jrnl->clear();
            jrnl->append(jrnlTxt);
            jrnl->append(QString());
            jrnl->append(usrText);
        }
        else
            jrnl->append(jrnlTxt);
        viewport->moveLsrhead(pos);
        shapeTransformChaged(BotSocket::ENST_LSRHEAD);
    }

    void gripPositionChanged(const BotSocket::SBotPosition &pos) final {
        const QString jrnlTxt = MainWindow::tr("Grip: %1\t-->\tx: %2 y: %3 z: %4 "
                                               "α: %5 β: %6 γ: %7")
                .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                .arg(pos.globalPos.x     , 12, 'f', 6, QChar('0'))
                .arg(pos.globalPos.y     , 12, 'f', 6, QChar('0'))
                .arg(pos.globalPos.z     , 12, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.x, 12, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.y, 12, 'f', 6, QChar('0'))
                .arg(pos.globalRotation.z, 12, 'f', 6, QChar('0'));
        if(!usrText.isEmpty()) {
            jrnl->clear();
            jrnl->append(jrnlTxt);
            jrnl->append(QString());
            jrnl->append(usrText);
        }
        else
            jrnl->append(jrnlTxt);
        viewport->moveGrip(pos);
        shapeTransformChaged(BotSocket::ENST_GRIP);
        if (viewport->getBotState() == BotSocket::ENBS_ATTACHED)
            shapeTransformChaged(BotSocket::ENST_PART);
    }

    void shapeCalibrationChanged(const BotSocket::EN_ShapeType shType, const BotSocket::SBotPosition &pos)
    {
        if (std::isnan(pos.globalPos.x) ||
            std::isnan(pos.globalPos.y) ||
            std::isnan(pos.globalPos.z) ||
            std::isnan(pos.globalRotation.x) ||
            std::isnan(pos.globalRotation.y) ||
            std::isnan(pos.globalRotation.z))
            return;

        viewport->shapeCalibrationChanged(shType, pos);
    }
    void shapeTransformChanged(const BotSocket::EN_ShapeType shType, const gp_Trsf &transform)
    {
        viewport->shapeTransformChanged(shType, transform);
    }

    const TopoDS_Shape& getShape(const BotSocket::EN_ShapeType shType) const final {
        using namespace BotSocket;
        switch(shType) {
            case ENST_DESK   : return viewport->getDeskShape();
            case ENST_PART   : return viewport->getPartShape();
            case ENST_LSRHEAD: return viewport->getLsrheadShape();
            case ENST_GRIP   : return viewport->getGripShape();
        }
        static const TopoDS_Shape sh;
        return sh;
    }

    const gp_Trsf& getShapeTransform(const BotSocket::EN_ShapeType shType) const final {
        using namespace BotSocket;
        switch(shType) {
            case ENST_DESK   : return viewport->getDeskTransform();
            case ENST_PART   : return viewport->getPartTransform();
            case ENST_LSRHEAD: return viewport->getLsrheadTransform();
            case ENST_GRIP   : return viewport->getGripTransform();
        }
        static const gp_Trsf trsf;
        return trsf;
    }

    void updateUiState() {
        jrnl->clear();
        switch(viewport->getUiState())
        {
            using namespace GUI_TYPES;

            case ENUS_CALIBRATION:
            case ENUS_TASK_EDITING:
                if (viewport->getCalibResult() == BotSocket::ENCR_FALL) {
                    usrText = MainWindow::tr(" Выполните калибровку");
                    jrnl->setText(usrText);
                    btnStart->setEnabled(false);
                    btnStart->setToolTip(usrText);
                }
                else if (viewport->getTaskPoints().empty()) {
                    usrText = MainWindow::tr(" Добавьте задание");
                    jrnl->setText(usrText);
                    btnStart->setEnabled(false);
                    btnStart->setToolTip(usrText);
                }
                else {
                    usrText = MainWindow::tr(" Готов");
                    btnStart->setEnabled(true);
                    btnStart->setToolTip(MainWindow::tr("Пуск"));
                }
                break;

            case ENUS_BOT_WORKED:
            default:
                btnStart->setEnabled(false);
                btnStart->setToolTip(MainWindow::tr("Занят"));
                break;
        }

        if (viewport->getCalibResult() == BotSocket::ENCR_FALL) {
            usrText = MainWindow::tr(" Выполните калибровку");
            jrnl->setText(usrText);
            btnStart->setEnabled(false);
            btnStart->setToolTip(usrText);
        }
        else if (viewport->getTaskPoints().empty()) {
            usrText = MainWindow::tr(" Добавьте задание");
            jrnl->setText(usrText);
            btnStart->setEnabled(false);
            btnStart->setToolTip(usrText);
        }
        else if (viewport->getUiState() == GUI_TYPES::ENUS_TASK_EDITING) {
            usrText = MainWindow::tr(" Готов");
            btnStart->setEnabled(true);
            btnStart->setToolTip(MainWindow::tr("Пуск"));
        }
    }

    void uiStateChanged() final {
        updateUiState();
    }

    void calibrationChanged() final {
        updateUiState();
    }

    void tasksChanged() final {
        updateUiState();
    }

    void homePointsChanged() final {
        updateUiState();
    }

    void makePartSnapshot(const char *fname) final {
        viewport->makePartSnapshot(fname);
    }

    void snapshotCalibrationDataRecieved(const gp_Vec &globalDelta) final {
        viewport->makeCorrectionBySnapshot(globalDelta);
    }

    bool execSnapshotCalibrationWarning() final {
        return QMessageBox::warning(viewport,
                                    MainWindow::tr("Ошибка калибровки"),
                                    MainWindow::tr("Не удалось определить положение детали. "
                                            "Продолжить обработку без смещения детали?"),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No)
                == QMessageBox::Yes;
    }

private:
    CMainViewport *viewport;
    QTextEdit *jrnl;
    QAction *btnStart;
    QString usrText;
};



class MainWindowPrivate
{
    friend class MainWindow;

private:
    MainWindowPrivate() :
        settingsStorage(&emptySettingsStorage),
        stateLamp(new QLabel()),
        attachLamp(new QLabel()),
        lampTm(new QTimer()) {
        lampTm->setSingleShot(false);
        lampTm->setInterval(STATE_LAMP_UPDATE_INTERVAL);
        lampTm->start();
    }

    ~MainWindowPrivate() {
        delete lampTm;
    }

    void setMSAA(const GUI_TYPES::TMSAA value, CMainViewport &view) {
        for(auto pair : mapMsaa) {
            pair.second->blockSignals(true);
            pair.second->setChecked(pair.first == value);
            pair.second->blockSignals(false);
        }
        view.setMSAA(value);
    }

    void initToolBar(QToolBar *tBar) {
        const QPixmap red =
                QPixmap(":/Lamps/Data/Lamps/red.png").scaled(tBar->iconSize(),
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
        attachActions << tBar->addWidget(txtAttach);
        attachLamp->setParent(tBar);
        attachLamp->setPixmap(red);
        attachActions << tBar->addWidget(attachLamp);
    }

    void updateBotLamps(const QSize iconSize, const BotSocket::EN_BotState state)
    {
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

private:
    std::map <GUI_TYPES::TMSAA, QAction *> mapMsaa;

    CEmptySettingsStorage emptySettingsStorage;
    CAbstractSettingsStorage *settingsStorage;

    QLabel * const stateLamp, * const attachLamp;
    QList <QAction *> attachActions;
    QTimer * const lampTm;
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
    connect(d_ptr->lampTm, &QTimer::timeout, this, &MainWindow::slUpdateBotLamps);

    configMenu();
    configToolBar();

    //Callib
    connect(ui->wSettings, SIGNAL(sigCalcCalibration()), SLOT(slCallibCalc()));
    connect(ui->wSettings, SIGNAL(sigApplyRequest()), SLOT(slCallibApply()));

    ui->teJrnl->document()->setMaximumBlockCount(MAX_JRNL_ROW_COUNT);
}

MainWindow::~MainWindow()
{
    delete d_ptr;
    delete ui;
}

inline static TopoDS_Shape loadShape(const std::string fName) {
    TopoDS_Shape result;
    QFile modelFile(QString::fromStdString(fName));
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

    ui->mainView->addSubscriber(&d_ptr->uiIface);
}

void MainWindow::setSettingsStorage(CAbstractSettingsStorage &storage)
{
    d_ptr->settingsStorage = &storage;

    ui->mainView->setPartModel   (loadShape(storage.loadModelPath(GUI_TYPES::ENMP_PART   )));
    ui->mainView->setDeskModel   (loadShape(storage.loadModelPath(GUI_TYPES::ENMP_DESK   )));
    ui->mainView->setLsrheadModel(loadShape(storage.loadModelPath(GUI_TYPES::ENMP_LSRHEAD)));
    ui->mainView->setGripModel   (loadShape(storage.loadModelPath(GUI_TYPES::ENMP_GRIP   )));

    ui->mainView->setShading(true);
    ui->mainView->setUiState(GUI_TYPES::ENUS_TASK_EDITING);

    const GUI_TYPES::SGuiSettings settings = storage.loadGuiSettings();
    for(auto pair : d_ptr->mapMsaa) {
        pair.second->blockSignals(true);
        pair.second->setChecked(pair.first == settings.msaa);
        pair.second->blockSignals(false);
    }
    ui->wSettings->initFromGuiSettings(settings);
    ui->mainView->setGuiSettings(settings);
    ui->mainView->fitInView();

    foreach(QAction * const action, d_ptr->attachActions)
        action->setVisible(settings.gripVis);
}

void MainWindow::setBotSocket(CAbstractBotSocket &botSocket)
{
    using namespace BotSocket;

    d_ptr->uiIface.setBotSocket(botSocket);
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

void MainWindow::slCalibPointsOrderDlg()
{
    CCalibPointsOrderDialog dialog(ui->mainView->getCallibrationPoints());
    if (dialog.exec() == QDialog::Accepted)
        ui->mainView->setCalibrationPoints(dialog.getCalibPoints());
}

void MainWindow::slTaskPointsOrderDlg()
{
    CTaskPointsOrderDialog dialog(ui->mainView->getTaskPoints());
    if (dialog.exec() == QDialog::Accepted)
        ui->mainView->setTaskPoints(dialog.getTaskPoints());
}

void MainWindow::slPathPointsOrderDlg()
{
    CPathPointsOrderDialog dialog(ui->mainView->getHomePoints());
    if (dialog.exec() == QDialog::Accepted)
        ui->mainView->setHomePoints(dialog.getHomePoints());
}

void MainWindow::slSavePoints()
{
    QString fName = QFileDialog::getSaveFileName(this,
                                                 tr("Сохранение задания"),
                                                 QString(),
                                                 tr("Task (*.task)"));
    if (!fName.isEmpty())
    {
        const QStringList ls = fName.split(".");
        if (ls.last() != QString("task"))
            fName.append(".task");

        CJsonFilePointsSaver saver;
        saver.setFileName(fName.toLatin1());
        const bool res = saver.savePoints(ui->mainView->getTaskPoints(),
                                          ui->mainView->getHomePoints());
        if (!res)
            QMessageBox::critical(this,
                                  tr("Сохранение задания"),
                                  tr("Не удалось сохранить задание"));
    }
}

void MainWindow::slLoadPoints()
{
    const QString fName = QFileDialog::getOpenFileName(this,
                                                       tr("Загрузка задания"),
                                                       QString(),
                                                       tr("Task (*.task)"));
    if (!fName.isEmpty())
    {
        std::vector <GUI_TYPES::STaskPoint> taskPoints;
        std::vector <GUI_TYPES::SHomePoint> homePoints;
        CJsonFilePointsSaver saver;
        saver.setFileName(fName.toLatin1());
        const bool bRes = saver.loadPoints(taskPoints, homePoints);
        if (!bRes || (taskPoints.empty() && homePoints.empty()))
        {
            QMessageBox::critical(this,
                                  tr("Загрузка задания"),
                                  tr("Не удалось загрузить задание"));
        }
        else
        {
            ui->mainView->setTaskPoints(taskPoints);
            ui->mainView->setHomePoints(homePoints);
        }
    }
}

void MainWindow::slResetPoints()
{
    ui->mainView->setTaskPoints(std::vector<GUI_TYPES::STaskPoint> ());
    ui->mainView->setHomePoints(std::vector<GUI_TYPES::SHomePoint> ());
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

void MainWindow::slCallibCalc()
{
    const BotSocket::EN_CalibResult calibRes =
            d_ptr->uiIface.execCalibration(ui->mainView->getCallibrationLocalPoints());
    if(calibRes == BotSocket::ENCR_OK)
        QMessageBox::information(this,
                              tr("Калибровка завершена"),
                              tr("Калибровка завершена"));
    else
        QMessageBox::warning(this,
                              tr("Ошибка калибровки"),
                              tr("Проверьте правильность точек калибровки"));

    ui->wSettings->initFromGuiSettings(ui->mainView->getGuiSettings());
    ui->mainView->setCalibResult(calibRes);
}

void MainWindow::slPartPrntScr()
{
    const GUI_TYPES::SGuiSettings oldSettings = ui->mainView->getGuiSettings();
    const GUI_TYPES::SGuiSettings newSettings = ui->mainView->partPrntScr();
    const bool needSave =
            oldSettings.snapshotScale  != newSettings.snapshotScale ||
            oldSettings.snapshotWidth  != newSettings.snapshotWidth ||
            oldSettings.snapshotHeight != newSettings.snapshotHeight;
    if (needSave &&
        QMessageBox::question(this,
                              tr("Снимок детали"),
                              tr("Сохранить новые настройки снимка детали?")) == QMessageBox::Yes)
    {
        ui->mainView->setSnapshotParams(newSettings.snapshotScale,
                                        newSettings.snapshotWidth,
                                        newSettings.snapshotHeight);
        d_ptr->settingsStorage->saveGuiSettings(newSettings);
        ui->wSettings->initFromGuiSettings(newSettings);
    }
}

void MainWindow::slCallibApply()
{
    const GUI_TYPES::SGuiSettings settings = ui->wSettings->getChangedSettings();
    ui->mainView->setGuiSettings(settings);
    d_ptr->uiIface.shapeTransformChaged(BotSocket::ENST_DESK   );
    d_ptr->uiIface.shapeTransformChaged(BotSocket::ENST_LSRHEAD);
    d_ptr->uiIface.shapeTransformChaged(BotSocket::ENST_PART   );
    d_ptr->uiIface.shapeTransformChaged(BotSocket::ENST_GRIP   );

    d_ptr->settingsStorage->saveGuiSettings(settings);

    foreach(QAction * const action, d_ptr->attachActions)
        action->setVisible(settings.gripVis);
}

void MainWindow::slStart()
{
    ui->mainView->setUiState(GUI_TYPES::ENUS_BOT_WORKED);
    d_ptr->uiIface.startTasks(ui->mainView->getHomePoints(),
                              ui->mainView->getTaskPoints());
}

void MainWindow::slStop()
{
    d_ptr->uiIface.stopTasks();
    const GUI_TYPES::EN_UiStates state = ui->actionCalib->isChecked()
            ? GUI_TYPES::ENUS_CALIBRATION
            : GUI_TYPES::ENUS_TASK_EDITING;
    ui->mainView->setUiState(state);
}

void MainWindow::slUpdateBotLamps()
{
    d_ptr->updateBotLamps(ui->toolBar->iconSize(), ui->mainView->getBotState());
}

void MainWindow::configMenu()
{
    //Menu "File"
    connect(ui->actionImport, SIGNAL(triggered(bool)), SLOT(slImport()));
    connect(ui->actionCalibPoints, SIGNAL(triggered(bool)), SLOT(slCalibPointsOrderDlg()));
    connect(ui->actionTaskPoints, SIGNAL(triggered(bool)), SLOT(slTaskPointsOrderDlg()));
    connect(ui->actionPathPoints, SIGNAL(triggered(bool)), SLOT(slPathPointsOrderDlg()));
    connect(ui->actionSavePoints, SIGNAL(triggered(bool)), SLOT(slSavePoints()));
    connect(ui->actionLoadPoints, SIGNAL(triggered(bool)), SLOT(slLoadPoints()));
    connect(ui->actionResetPoints, SIGNAL(triggered(bool)), SLOT(slResetPoints()));
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
    //PrntScr
    connect(ui->actionPartPrntScr, SIGNAL(triggered(bool)), SLOT(slPartPrntScr()));

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
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(QIcon(":/icons/Data/Icons/25234_cd_folder_orange_icon.png"),
                           ui->actionSavePoints->text(),
                           ui->actionSavePoints,
                           SLOT(trigger()));
    ui->toolBar->addAction(QIcon(":/icons/Data/Icons/25252_folder_orange_icon.png"),
                           ui->actionLoadPoints->text(),
                           ui->actionLoadPoints,
                           SLOT(trigger()));
    ui->toolBar->addAction(QIcon(":/icons/Data/Icons/25481_delete_pen_signature_icon.png"),
                           ui->actionResetPoints->text(),
                           ui->actionResetPoints,
                           SLOT(trigger()));

    //Stretch
    QLabel * const strech = new QLabel(" ", ui->toolBar);
    strech->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->toolBar->addWidget(strech);
    d_ptr->uiIface.btnStart = ui->toolBar->addAction(QIcon(":/icons/Data/Icons/play.png"),
                                                     tr("Старт"),
                                                     this,
                                                     SLOT(slStart()));
    ui->toolBar->addAction(QIcon(":/icons/Data/Icons/stop.png"),
                           tr("Стоп"),
                           this,
                           SLOT(slStop()));

    //State and attach
    d_ptr->initToolBar(ui->toolBar);
}

