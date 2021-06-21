#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QLabel>

#include <OpenGl_GraphicDriver.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>

#include <AIS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <AIS_TextLabel.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <AIS_ViewCube.hxx>
#include <IntCurvesFace_ShapeIntersector.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Standard_Version.hxx>
#include <gp_Quaternion.hxx>
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>

#include "ModelLoader/cmodelloaderfactorymethod.h"
#include "ModelLoader/csteploader.h"

#include "Primitives/cbotcross.h"
#include "Primitives/claservec.h"

#include "cabstractguisettings.h"

static constexpr double DEGREE_K = M_PI / 180.;

static const Quantity_Color BG_CLR   = Quantity_Color(.7765,  .9, 1.  , Quantity_TOC_RGB);
static const Quantity_Color TXT_CLR  = Quantity_Color(  .15, .15, 0.15, Quantity_TOC_RGB);
static const Quantity_Color FACE_CLR = Quantity_Color(0.1, 0.1, 0.1, Quantity_TOC_RGB);

static constexpr int MAX_JRNL_ROW_COUNT = 15000;

static const Standard_Integer Z_LAYER = 100;
class CModelMover;


class MainWindowPrivate
{
    friend class MainWindow;
    friend class CModelMover;

private:
    MainWindowPrivate() :
        guiSettings(&emptySettings),
        zLayerId(Z_LAYER),
        bCalibEnabled(false)
    { }

    void init(OpenGl_GraphicDriver &driver) {
        viewer = new V3d_Viewer(&driver);
        viewer->SetDefaultViewSize(1000.);
        viewer->SetComputedMode(Standard_True);
        viewer->SetDefaultComputedMode(Standard_True);
        viewer->SetDefaultLights();
        viewer->SetLightOn();

        context = new AIS_InteractiveContext(viewer);
        context->SetAutoActivateSelection(false);

        Handle(Prs3d_Drawer) drawer = context->DefaultDrawer();
        Handle(Prs3d_DatumAspect) datum = drawer->DatumAspect();

#if OCC_VERSION_HEX >= 0x070600
        datum->TextAspect(Prs3d_DatumParts_XAxis)->SetColor(TXT_CLR);
        datum->TextAspect(Prs3d_DatumParts_YAxis)->SetColor(TXT_CLR);
        datum->TextAspect(Prs3d_DatumParts_ZAxis)->SetColor(TXT_CLR);
#else
        datum->TextAspect()->SetColor(TXT_CLR);
#endif

        Handle(Prs3d_LineAspect) lAspect = drawer->FaceBoundaryAspect();
        lAspect->SetColor(FACE_CLR);
        drawer->SetFaceBoundaryAspect(lAspect);
        drawer->SetFaceBoundaryDraw(Standard_True);

        viewer->AddZLayer(zLayerId);
        Graphic3d_ZLayerSettings settings = viewer->ZLayerSettings(zLayerId);
        settings.SetEnableDepthTest(Standard_False);
        viewer->SetZLayerSettings(zLayerId, settings);

        //load gripModel
        QFile gripFile(":/Models/Data/Models/MHZ2_16D_grip.stp");
        if (gripFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            const QByteArray stepData = gripFile.readAll();
            CStepLoader loader;
            gripModel = loader.loadFromBinaryData(stepData.constData(), static_cast <size_t> (stepData.size()));
        }

        QFile defaultModelFile(":/Models/Data/Models/turbine_blade.stp");
        if (defaultModelFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            const QByteArray stepData = defaultModelFile.readAll();
            CStepLoader loader;
            curModel = loader.loadFromBinaryData(stepData.constData(), static_cast <size_t> (stepData.size()));
        }
    }

    static gp_Trsf calc_transform(const gp_Trsf &base_transform,
                                  const gp_Vec &model_translation,
                                  const gp_Vec &model_center,
                                  double scaleFactor,
                                  double alpha_off, double beta_off, double gamma_off,
                                  double alpha_cur = 0.0, double beta_cur = 0.0, double gamma_cur = 0.0) {
        gp_Trsf trsfTr3 = base_transform;
        trsfTr3.SetTranslation(model_translation + model_center);

        gp_Trsf trsfSc = base_transform;
        if (scaleFactor == 0.)
            scaleFactor = 1.;
        trsfSc.SetScale(gp_Pnt(), scaleFactor);

        gp_Trsf trsfTr2 = base_transform;
        trsfTr2.SetTranslation(model_center);

        gp_Trsf trsfRoff = base_transform;
        gp_Quaternion qoff;
        qoff.SetEulerAngles(gp_Extrinsic_XYZ,
                         alpha_off * DEGREE_K,
                         beta_off  * DEGREE_K,
                         gamma_off * DEGREE_K);
        trsfRoff.SetRotation(qoff);

        gp_Trsf trsfRcur = base_transform;
        gp_Quaternion qcur;
        qcur.SetEulerAngles(gp_Extrinsic_XYZ,
                         alpha_cur * DEGREE_K,
                         beta_cur  * DEGREE_K,
                         gamma_cur * DEGREE_K);
        trsfRcur.SetRotation(qcur);

        gp_Trsf trsfTr1 = base_transform;
        trsfTr1.SetTranslation(-model_center);

        return trsfTr3 *
               trsfSc *
               trsfTr2 *
               trsfRcur *
               trsfRoff *
               trsfTr1;
    }

    AIS_InteractiveObject* createCube() const {
        AIS_ViewCube * const aViewCube = new AIS_ViewCube();
        aViewCube->SetDrawEdges(Standard_False);
        aViewCube->SetDrawVertices(Standard_False);
        aViewCube->SetBoxTransparency(1.);
        aViewCube->AIS_InteractiveObject::SetColor(TXT_CLR);
        TCollection_AsciiString emptyStr;
        aViewCube->SetBoxSideLabel(V3d_Xpos, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Ypos, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Zpos, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Xneg, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Yneg, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Zneg, emptyStr);
        return aViewCube;
    }

    AIS_InteractiveObject* createGlobalRope() const {
        Geom_Axis2Placement coords(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.), gp_Dir(1., 0., 0.));
        Handle(Geom_Axis2Placement) axis = new Geom_Axis2Placement(coords);
        AIS_Trihedron * const aTrih = new AIS_Trihedron(axis);
        return aTrih;
    }

    void updateModelsDefaultPosition(const bool shading) {
        context->RemoveAll(Standard_False);

        //Draw AIS_ViewCube
        Handle(AIS_InteractiveObject) aViewCube = createCube();
        context->SetDisplayMode(aViewCube, 1, Standard_False);
        context->Display(aViewCube, Standard_False);

        if (bCalibEnabled) {
            Handle(AIS_InteractiveObject) aRope = createGlobalRope();
            context->SetDisplayMode(aRope, 1, Standard_False);
            context->Display(aRope, Standard_False);
        }

        //The Part
        {
            gp_Trsf loc = calc_transform(curModel.Location().Transformation(),
                                         gp_Vec(guiSettings->getPartTrX(),
                                                guiSettings->getPartTrY(),
                                                guiSettings->getPartTrZ()),
                                         gp_Vec(guiSettings->getPartCenterX(),
                                                guiSettings->getPartCenterY(),
                                                guiSettings->getPartCenterZ()),
                                         guiSettings->getPartScale(),
                                         guiSettings->getPartRotationX(),
                                         guiSettings->getPartRotationY(),
                                         guiSettings->getPartRotationZ());


            ais_mdl = new AIS_Shape(curModel);
            context->SetDisplayMode(ais_mdl, shading ? 1 : 0, Standard_False);
            context->Display(ais_mdl, Standard_False);
            context->SetLocation(ais_mdl, loc);
        }

        viewer->Redraw();
    }

    bool load(const QString &fName, CAbstractModelLoader &loader, const bool shading) {
        const TopoDS_Shape shape = loader.load(fName.toStdString().c_str());
        curModel = shape;
        updateModelsDefaultPosition(shading);
        return !curModel.IsNull();
    }

    void setMSAA(const GUI_TYPES::TMSAA value, CMainViewport &view) {
        for(auto pair : mapMsaa) {
            pair.second->blockSignals(true);
            pair.second->setChecked(pair.first == value);
            pair.second->blockSignals(false);
        }
        view.setMSAA(value);
    }

private:
    CEmptyGuiSettings emptySettings;
    CAbstractGuiSettings *guiSettings;

    Handle(V3d_Viewer)             viewer;
    Handle(AIS_InteractiveContext) context;
    Standard_Integer zLayerId;

    TopoDS_Shape curModel;
    Handle(AIS_Shape) ais_mdl;
    TopoDS_Shape gripModel;
    Handle(AIS_Shape) ais_grip;

    bool bCalibEnabled;

    std::map <GUI_TYPES::TMSAA, QAction *> mapMsaa;
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
    d_ptr->init(driver);
    ui->mainView->init(*d_ptr->context);
    ui->mainView->setBackgroundColor(BG_CLR);
    ui->mainView->setStatsVisible(ui->actionFPS->isChecked());
}

void MainWindow::setSettings(CAbstractGuiSettings &settings)
{
    d_ptr->guiSettings = &settings;

    d_ptr->setMSAA(settings.getMsaa(), *ui->mainView);
    ui->wSettings->initFromGuiSettings(settings);
    d_ptr->updateModelsDefaultPosition(ui->actionShading->isChecked());
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
        if (d_ptr->load(fName, loader, ui->actionShading->isChecked()))
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
    d_ptr->updateModelsDefaultPosition(enabled);
}

void MainWindow::slShowCalibWidget(bool enabled)
{
    ui->dockSettings->setVisible(enabled);
    d_ptr->bCalibEnabled = enabled;
    d_ptr->updateModelsDefaultPosition(ui->actionShading->isChecked());
    ui->mainView->setCalibEnabled(enabled);
    if (enabled)
    {
        d_ptr->context->Load(d_ptr->ais_mdl, -1);
        d_ptr->context->Activate(d_ptr->ais_mdl);
    }
    else
        d_ptr->context->Deactivate(d_ptr->ais_mdl);
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
    d_ptr->setMSAA(msaa, *ui->mainView);
    d_ptr->guiSettings->setMsaa(msaa);
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
    ui->wSettings->applyToGuiSettings(*d_ptr->guiSettings);
    d_ptr->updateModelsDefaultPosition(ui->actionShading->isChecked());
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

