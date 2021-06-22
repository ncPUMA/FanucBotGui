#include "cmainviewport.h"

#include <QDebug>
#include <QMouseEvent>

#include <AIS_ViewController.hxx>

#include <OpenGl_GraphicDriver.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Standard_Version.hxx>
#include <Graphic3d_ZLayerSettings.hxx>

#include <gp_Quaternion.hxx>
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>

#include <AIS_ViewCube.hxx>
#include <AIS_Shape.hxx>
#include <Geom_CartesianPoint.hxx>
#include <AIS_Point.hxx>
#include <AIS_TextLabel.hxx>
#include <TopoDS_Shape.hxx>

#include "caspectwindow.h"
#include "sguisettings.h"

static const Quantity_Color BG_CLR   = Quantity_Color( .7765,  .9 , 1.  , Quantity_TOC_RGB);
static const Quantity_Color TXT_CLR  = Quantity_Color( .15  ,  .15, 0.15, Quantity_TOC_RGB);
static const Quantity_Color FACE_CLR = Quantity_Color(0.1   , 0.1 , 0.1 , Quantity_TOC_RGB);

static constexpr double DEGREE_K = M_PI / 180.;

static const Standard_Integer Z_LAYER = 100;

class CMainViewportPrivate : public AIS_ViewController
{
    friend class CMainViewport;

    CMainViewportPrivate(CMainViewport * const qptr) :
        q_ptr(qptr),
        zLayerId(Z_LAYER),
        bCalibEnabled(false),
        pnt(new AIS_Point(new Geom_CartesianPoint(gp_Pnt()))),
        pntLbl(new AIS_TextLabel())
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
        Graphic3d_ZLayerSettings zSettings = viewer->ZLayerSettings(zLayerId);
        zSettings.SetEnableDepthTest(Standard_False);
        viewer->SetZLayerSettings(zLayerId, zSettings);

        v3dView = context->CurrentViewer()->CreateView().get();

        aspect = new CAspectWindow(*q_ptr);
        v3dView->SetWindow(aspect);
        if (!aspect->IsMapped())
        {
          aspect->Map();
        }

        myMouseGestureMap.Clear();
        myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Pan);
        myMouseGestureMap.Bind(Aspect_VKeyMouse_RightButton, AIS_MouseGesture_RotateOrbit);

        SetAllowRotation(Standard_True);

        v3dView->ChangeRenderingParams().IsAntialiasingEnabled = Standard_True;
        v3dView->SetBackgroundColor(BG_CLR);
        v3dView->MustBeResized();
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
            gp_Trsf loc = calc_transform(mainModel.Location().Transformation(),
                                         gp_Vec(guiSettings.partTrX,
                                                guiSettings.partTrY,
                                                guiSettings.partTrZ),
                                         gp_Vec(guiSettings.partCenterX,
                                                guiSettings.partCenterY,
                                                guiSettings.partCenterZ),
                                         guiSettings.partScale,
                                         guiSettings.partRotationX,
                                         guiSettings.partRotationY,
                                         guiSettings.partRotationZ);


            ais_mdl = new AIS_Shape(mainModel);
            context->SetDisplayMode(ais_mdl, shading ? 1 : 0, Standard_False);
            context->Display(ais_mdl, Standard_False);
            context->SetLocation(ais_mdl, loc);
        }

        viewer->Redraw();
    }

    void paintEvent() {
        v3dView->InvalidateImmediate();
        FlushViewEvents(context, v3dView, Standard_True);
    }

    void resizeEvent() {
        v3dView->MustBeResized();
    }

    void fitInView() {
        v3dView->FitAll();
        v3dView->ZFitAll();
        v3dView->Redraw();
    }

    void setMSAA(const GUI_TYPES::TMSAA msaa) {
        Q_ASSERT(!v3dView.IsNull());
        guiSettings.msaa = msaa;
        v3dView->ChangeRenderingParams().NbMsaaSamples = msaa;
        v3dView->Redraw();
    }


    CMainViewport * const q_ptr;

    Handle(AIS_InteractiveContext) context;
    Handle(V3d_Viewer)             viewer;
    Handle(V3d_View)               v3dView;
    Handle(CAspectWindow)          aspect;

    Standard_Integer zLayerId;

    SGuiSettings guiSettings;

    TopoDS_Shape mainModel;
    Handle(AIS_Shape) ais_mdl;
    TopoDS_Shape gripModel;
    Handle(AIS_Shape) ais_grip;

    bool bCalibEnabled;
    Handle(AIS_Point) pnt;
    Handle(AIS_TextLabel) pntLbl;
};



CMainViewport::CMainViewport(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CMainViewportPrivate(this))
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    setBackgroundRole(QPalette::NoRole);
    setFocusPolicy(Qt::StrongFocus);
}

CMainViewport::~CMainViewport()
{
    delete d_ptr;
}

void CMainViewport::init(OpenGl_GraphicDriver &driver)
{
    d_ptr->init(driver);
}

void CMainViewport::setGuiSettings(const SGuiSettings &settings)
{
    d_ptr->guiSettings = settings;
    d_ptr->v3dView->ChangeRenderingParams().NbMsaaSamples = settings.msaa;
}

SGuiSettings CMainViewport::getGuiSettings() const
{
    return d_ptr->guiSettings;
}

void CMainViewport::setMSAA(const GUI_TYPES::TMSAA msaa)
{
    d_ptr->setMSAA(msaa);
}

GUI_TYPES::TMSAA CMainViewport::getMSAA() const
{
    return static_cast <GUI_TYPES::TMSAA> (d_ptr->v3dView->RenderingParams().NbMsaaSamples);
}

void CMainViewport::setStatsVisible(const bool value)
{
    d_ptr->v3dView->ChangeRenderingParams().ToShowStats = value;
}

void CMainViewport::fitInView()
{
    d_ptr->fitInView();
}

void CMainViewport::setCoord(const GUI_TYPES::TCoordSystem type)
{
    V3d_TypeOfOrientation orientation = V3d_XposYnegZpos;
    if (type == GUI_TYPES::ENCS_LEFT)
        orientation = V3d_XposYnegZneg;
    d_ptr->v3dView->SetProj(orientation, Standard_False);
    d_ptr->v3dView->Redraw();
}

void CMainViewport::setCalibEnabled(bool enabled)
{
    d_ptr->bCalibEnabled = enabled;
    if (enabled)
    {
        d_ptr->context->Load(d_ptr->ais_mdl, -1);
        d_ptr->context->Activate(d_ptr->ais_mdl);
    }
    else
        d_ptr->context->Deactivate(d_ptr->ais_mdl);
}

void CMainViewport::setMainModel(const TopoDS_Shape &shape)
{
    d_ptr->mainModel = shape;
}

void CMainViewport::setGripModel(const TopoDS_Shape &shape)
{
    d_ptr->gripModel = shape;
}

void CMainViewport::updateModelsDefaultPosition(const bool shading)
{
    d_ptr->updateModelsDefaultPosition(shading);
}

QPaintEngine *CMainViewport::paintEngine() const
{
    return nullptr;
}

void CMainViewport::paintEvent(QPaintEvent *)
{
    d_ptr->paintEvent();
}

void CMainViewport::resizeEvent(QResizeEvent *)
{
    d_ptr->resizeEvent();
}

//! Map Qt buttons bitmask to virtual keys.
inline static Aspect_VKeyMouse qtMouseButtons2VKeys(Qt::MouseButtons theButtons)
{
    Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
    if ((theButtons & Qt::LeftButton) != 0)
    {
        aButtons |= Aspect_VKeyMouse_LeftButton;
    }
    if ((theButtons & Qt::MiddleButton) != 0)
    {
        aButtons |= Aspect_VKeyMouse_MiddleButton;
    }
    if ((theButtons & Qt::RightButton) != 0)
    {
        aButtons |= Aspect_VKeyMouse_RightButton;
    }
    return aButtons;
}

//! Map Qt mouse modifiers bitmask to virtual keys.
inline static Aspect_VKeyFlags qtMouseModifiers2VKeys(Qt::KeyboardModifiers theModifiers)
{
    Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
    if ((theModifiers & Qt::ShiftModifier) != 0)
    {
        aFlags |= Aspect_VKeyFlags_SHIFT;
    }
    if ((theModifiers & Qt::ControlModifier) != 0)
    {
        aFlags |= Aspect_VKeyFlags_CTRL;
    }
    if ((theModifiers & Qt::AltModifier) != 0)
    {
        aFlags |= Aspect_VKeyFlags_ALT;
    }
    return aFlags;
}

void CMainViewport::mousePressEvent(QMouseEvent *event)
{
    const Graphic3d_Vec2i aPnt(event->pos().x(), event->pos().y());
    const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(event->modifiers());
    if (d_ptr->UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(event->buttons()), aFlags, false))
        update();
}

void CMainViewport::mouseReleaseEvent(QMouseEvent *event)
{
    const Graphic3d_Vec2i aPnt(event->pos().x(), event->pos().y());
    const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(event->modifiers());
    if (d_ptr->UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(event->buttons()), aFlags, false))
        update();
}

void CMainViewport::mouseMoveEvent(QMouseEvent *event)
{
    const Graphic3d_Vec2i aNewPos(event->pos().x(), event->pos().y());
    if (d_ptr->UpdateMousePosition(aNewPos,
                                   qtMouseButtons2VKeys(event->buttons()),
                                   qtMouseModifiers2VKeys(event->modifiers()),
                                   false))
    {
        update();
    }

    if (d_ptr->bCalibEnabled)
    {
        Handle(StdSelect_ViewerSelector3d) selector = d_ptr->context->MainSelector();
        if (selector->NbPicked() > 0)
        {
            const gp_Pnt pick = selector->PickedPoint(1);
            d_ptr->pnt->SetComponent(new Geom_CartesianPoint(pick));
            if (d_ptr->context->IsDisplayed(d_ptr->pnt))
                d_ptr->context->Redisplay(d_ptr->pnt, Standard_False);
            else
                d_ptr->context->Display(d_ptr->pnt, Standard_False);
            const QString txt = QString("X:%1\nY:%2\nZ:%3")
                    .arg(pick.X())
                    .arg(pick.Y())
                    .arg(pick.Z());
            d_ptr->pntLbl->SetPosition(pick);
            d_ptr->pntLbl->SetText(txt.toLocal8Bit().constData());
            if (d_ptr->context->IsDisplayed(d_ptr->pntLbl))
                d_ptr->context->Redisplay(d_ptr->pntLbl, Standard_False);
            else
                d_ptr->context->Display(d_ptr->pntLbl, Standard_False);
            d_ptr->v3dView->Redraw();
        }
    }
//    Handle(SelectMgr_EntityOwner) owner = d_ptr->context->DetectedOwner();
//    if (owner)
//    {
//        Handle(AIS_Shape) shape = Handle(AIS_Shape)::DownCast(owner->Selectable());
//        if (shape)
//            qDebug() << shape->This();
//    }
}

void CMainViewport::wheelEvent(QWheelEvent *event)
{
    const Graphic3d_Vec2i aPos(event->pos().x(), event->pos().y());
    if (d_ptr->UpdateZoom(Aspect_ScrollDelta(aPos, event->delta() / 8)))
        update();
}
