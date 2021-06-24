#include "cmainviewport.h"

#include <vector>

#include <QDebug>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QVariant>

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

#include "Dialogs/caddcalibpointdialog.h"
#include "Dialogs/cbottaskdialogfacade.h"

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
        usrAction(GUI_TYPES::ENUA_NO),
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
//        context->SetAutoActivateSelection(false);

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

        ais_axis_cube = createCube();
        Geom_Axis2Placement coords(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.), gp_Dir(1., 0., 0.));
        Handle(Geom_Axis2Placement) axis = new Geom_Axis2Placement(coords);
        calibTrihedron = new AIS_Trihedron(axis);

        v3dView->ChangeRenderingParams().IsAntialiasingEnabled = Standard_True;
        v3dView->SetBackgroundColor(BG_CLR);
        v3dView->MustBeResized();
    }

    static gp_Trsf calc_transform(const gp_Vec &model_translation,
                                  const gp_Vec &model_center,
                                  double scaleFactor,
                                  double alpha_off, double beta_off, double gamma_off,
                                  double alpha_cur = 0.0, double beta_cur = 0.0, double gamma_cur = 0.0) {
        gp_Trsf trsfTr3;
        trsfTr3.SetTranslation(model_translation + model_center);

        gp_Trsf trsfSc;
        if (scaleFactor == 0.)
            scaleFactor = 1.;
        trsfSc.SetScale(gp_Pnt(), scaleFactor);

        gp_Trsf trsfTr2;
        trsfTr2.SetTranslation(model_center);

        gp_Trsf trsfRoff;
        gp_Quaternion qoff;
        qoff.SetEulerAngles(gp_Extrinsic_XYZ,
                         alpha_off * DEGREE_K,
                         beta_off  * DEGREE_K,
                         gamma_off * DEGREE_K);
        trsfRoff.SetRotation(qoff);

        gp_Trsf trsfRcur;
        gp_Quaternion qcur;
        qcur.SetEulerAngles(gp_Extrinsic_XYZ,
                         alpha_cur * DEGREE_K,
                         beta_cur  * DEGREE_K,
                         gamma_cur * DEGREE_K);
        trsfRcur.SetRotation(qcur);

        gp_Trsf trsfTr1;
        trsfTr1.SetTranslation(-model_center);

        return trsfTr3 *
               trsfSc *
               trsfTr2 *
               trsfRcur *
               trsfRoff *
               trsfTr1;
    }

    AIS_ViewCube* createCube() const {
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

    void setMainModel(const TopoDS_Shape &shape) {
        ais_mdl = new AIS_Shape(shape);
        calibTrihedron->SetSize(10.);
    }

    void updateModelsDefaultPosition(const bool shading) {
        context->RemoveAll(Standard_False);

        //Draw AIS_ViewCube
        context->Display(ais_axis_cube, Standard_False);
        context->SetDisplayMode(ais_axis_cube, 1, Standard_False);
        context->Deactivate(ais_axis_cube);

        if (usrAction == GUI_TYPES::ENUA_CALIBRATION) {
            //Draw calib trihedron
            context->Display(calibTrihedron, Standard_False);
            context->SetDisplayMode(calibTrihedron, 1, Standard_False);
            context->SetZLayer(calibTrihedron, zLayerId);
            context->Deactivate(calibTrihedron);
            //Draw points
            for(auto scpnt : calibPoints) {
                context->Display(scpnt.pnt, Standard_False);
                context->SetZLayer(scpnt.pntLbl, zLayerId);
                context->Display(scpnt.pntLbl, Standard_False);
                context->Deactivate(scpnt.pntLbl);
            }
        }

        //The Part
        {
            gp_Trsf loc = calc_transform(gp_Vec(guiSettings.partTrX,
                                                guiSettings.partTrY,
                                                guiSettings.partTrZ),
                                         gp_Vec(guiSettings.partCenterX,
                                                guiSettings.partCenterY,
                                                guiSettings.partCenterZ),
                                         guiSettings.partScale,
                                         guiSettings.partRotationX,
                                         guiSettings.partRotationY,
                                         guiSettings.partRotationZ);

            context->SetDisplayMode(ais_mdl, shading ? 1 : 0, Standard_False);
            context->Display(ais_mdl, Standard_False);
            context->SetLocation(ais_mdl, loc);
        }

        viewer->Redraw();
    }

    void drawVertexCursor() {
        Handle(SelectMgr_EntityOwner) owner = context->DetectedOwner();
        bool bShow = false;
        if (owner)
            bShow = Handle(AIS_Shape)::DownCast(owner->Selectable()) == ais_mdl;

        if (bShow) {
            Handle(StdSelect_ViewerSelector3d) selector = context->MainSelector();
            if (selector->NbPicked() > 0) {
                //cross
                const gp_Pnt pick = selector->PickedPoint(1);
                pnt->SetComponent(new Geom_CartesianPoint(pick));

                //label
                const QString txt = QString("  X: %1\n  Y: %2\n  Z: %3")
                        .arg(pick.X(), 10, 'f', 6, '0')
                        .arg(pick.Y(), 10, 'f', 6, '0')
                        .arg(pick.Z(), 10, 'f', 6, '0');
                pntLbl->SetPosition(pick);
                pntLbl->SetText(txt.toLocal8Bit().constData());

                //redraw
                if (context->IsDisplayed(pntLbl)) {
                    context->Redisplay(pnt, Standard_False);
                    context->Redisplay(pntLbl, Standard_False);
                }
                else {
                    context->Display(pnt, Standard_False);
                    context->SetZLayer(pnt, zLayerId);
                    context->Deactivate(pnt);
                    context->Display(pntLbl, Standard_False);
                    context->SetZLayer(pntLbl, zLayerId);
                    context->Deactivate(pntLbl);
                }
            }
        }
        else {
            context->Remove(pnt, Standard_False);
            context->Remove(pntLbl, Standard_False);
        }
    }

    void updateModelCursor() {
        switch(usrAction) {
            using namespace GUI_TYPES;

            case ENUA_CALIBRATION:
            case ENUA_ADD_TASK:
                drawVertexCursor();
                v3dView->Redraw();
                break;

            default:
                break;
        }
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

    void fillCalibContextMenu(QMenu &menu) {
        Handle(SelectMgr_EntityOwner) owner = context->DetectedOwner();
        if (owner) {
            Handle(AIS_InteractiveObject) curShape = Handle(AIS_InteractiveObject)::DownCast(owner->Selectable());
            if (curShape == ais_mdl)
                menu.addAction(CMainViewport::tr("Точка привязки"),
                               q_ptr,
                               &CMainViewport::slAddCalibPoint);
            else {
                size_t i = 0;
                for (auto scpnt : calibPoints) {
                    if (scpnt.pnt == curShape) {
                        menu.addAction(CMainViewport::tr("Изменить"),
                                       q_ptr,
                                       &CMainViewport::slChangeCalibPoint)->
                                setProperty("index", static_cast <qulonglong> (i));
                        menu.addAction(CMainViewport::tr("Удалить"),
                                       q_ptr,
                                       &CMainViewport::slRemoveCalibPoint)->
                                setProperty("index", static_cast <qulonglong> (i));;
                        break;
                    }
                    ++i;
                }
            }
        }
    }

    void fillTaskContextMenu(QMenu &menu) {
        Handle(SelectMgr_EntityOwner) owner = context->DetectedOwner();
        if (owner) {
            Handle(AIS_InteractiveObject) curShape = Handle(AIS_InteractiveObject)::DownCast(owner->Selectable());
            if (curShape == ais_mdl) {
                using namespace GUI_TYPES;
                menu.addAction(CMainViewport::tr("Перемещение в точку"),
                               q_ptr,
                               &CMainViewport::slAddTaskPoint)->setProperty("taskType", ENBTT_MOVE);
                menu.addAction(CMainViewport::tr("Сверление"),
                               q_ptr,
                               &CMainViewport::slAddTaskPoint)->setProperty("taskType", ENBTT_DRILL);;
                menu.addAction(CMainViewport::tr("Маркировка"),
                               q_ptr,
                               &CMainViewport::slAddTaskPoint)->setProperty("taskType", ENBTT_MARK);;
            }
            else {
                size_t i = 0;
                for (auto scpnt : taskPoints) {
                    if (scpnt.pnt == curShape) {
                        menu.addAction(CMainViewport::tr("Изменить"),
                                       q_ptr,
                                       &CMainViewport::slChangeTaskPoint)->
                                setProperty("index", static_cast <qulonglong> (i));
                        menu.addAction(CMainViewport::tr("Удалить"),
                                       q_ptr,
                                       &CMainViewport::slRemoveTaskPoint)->
                                setProperty("index", static_cast <qulonglong> (i));;
                        break;
                    }
                    ++i;
                }
            }
        }
    }


    CMainViewport * const q_ptr;

    Handle(AIS_InteractiveContext) context;
    Handle(V3d_Viewer)             viewer;
    Handle(V3d_View)               v3dView;
    Handle(CAspectWindow)          aspect;

    Standard_Integer zLayerId;

    SGuiSettings guiSettings;

    Handle(AIS_ViewCube) ais_axis_cube;
    Handle(AIS_Shape) ais_mdl;
    Handle(AIS_Shape) ais_grip;

    GUI_TYPES::TUsrAction usrAction;

    Handle(AIS_Trihedron) calibTrihedron;
    Handle(AIS_Point) pnt;
    Handle(AIS_TextLabel) pntLbl;

    QPoint rbPos;

    struct SCalibPoint
    {
        gp_Pnt botPos;
        Handle(AIS_Point) pnt;
        Handle(AIS_TextLabel) pntLbl;
    };
    std::vector <SCalibPoint> calibPoints;

    struct STaskPoint
    {
        GUI_TYPES::TBotTaskType taskType;
        gp_Pnt angles;
        Handle(AIS_Point) pnt;
        Handle(AIS_TextLabel) pntLbl;
    };
    std::vector <STaskPoint> taskPoints;
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

void CMainViewport::setUserAction(const GUI_TYPES::TUsrAction usrAction)
{
    d_ptr->usrAction = usrAction;
}

GUI_TYPES::TUsrAction CMainViewport::getUsrAction() const
{
    return d_ptr->usrAction;
}

void CMainViewport::setMainModel(const TopoDS_Shape &shape)
{
    d_ptr->setMainModel(shape);
}

void CMainViewport::setGripModel(const TopoDS_Shape &shape)
{
    d_ptr->ais_grip = new AIS_Shape(shape);
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

    if ((event->buttons() & Qt::RightButton) == Qt::RightButton)
        d_ptr->rbPos = event->globalPos();
}

void CMainViewport::mouseReleaseEvent(QMouseEvent *event)
{
    const Graphic3d_Vec2i aPnt(event->pos().x(), event->pos().y());
    const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(event->modifiers());
    if (d_ptr->UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(event->buttons()), aFlags, false))
        update();

    if (!d_ptr->rbPos.isNull()) {
        d_ptr->rbPos = QPoint();

        QMenu menu;
        switch(d_ptr->usrAction)
        {
            using namespace GUI_TYPES;

            case ENUA_CALIBRATION:
                d_ptr->fillCalibContextMenu(menu);
                break;

            case ENUA_ADD_TASK:
                d_ptr->fillTaskContextMenu(menu);
                break;

            default:
                break;
        }

        if (!menu.isEmpty())
            menu.exec(event->globalPos());
    }
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

    d_ptr->rbPos = QPoint();
    d_ptr->updateModelCursor();
}

void CMainViewport::wheelEvent(QWheelEvent *event)
{
    const Graphic3d_Vec2i aPos(event->pos().x(), event->pos().y());
    if (d_ptr->UpdateZoom(Aspect_ScrollDelta(aPos, event->delta() / 8)))
        update();
}

void CMainViewport::slAddCalibPoint()
{
    const gp_Pnt point = d_ptr->pnt->Component()->Pnt();
    CAddCalibPointDialog dialog(point, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        const gp_Pnt globalPos = dialog.getGlobalPos();
        CMainViewportPrivate::SCalibPoint scpnt;
        scpnt.botPos = dialog.getBotPos();
        scpnt.pnt = new AIS_Point(new Geom_CartesianPoint(globalPos));
        scpnt.pntLbl = new AIS_TextLabel();
        scpnt.pntLbl->SetPosition(globalPos);
        const QString txt = QString("  C%1").arg(d_ptr->calibPoints.size() + 1);
        scpnt.pntLbl->SetText(txt.toLocal8Bit().constData());
        d_ptr->calibPoints.push_back(scpnt);
        d_ptr->context->Display(scpnt.pnt, Standard_False);
        d_ptr->context->SetZLayer(scpnt.pntLbl, d_ptr->zLayerId);
        d_ptr->context->Display(scpnt.pntLbl, Standard_False);
        d_ptr->context->Deactivate(scpnt.pntLbl);
        d_ptr->viewer->Redraw();
    }
}

void CMainViewport::slChangeCalibPoint()
{
    if (sender())
    {
        const size_t index = static_cast <size_t> (sender()->property("index").toULongLong());
        if (index < d_ptr->calibPoints.size())
        {
            CMainViewportPrivate::SCalibPoint &scpnt = d_ptr->calibPoints[index];
            gp_Pnt globalPos = scpnt.pnt->Component()->Pnt();
            CAddCalibPointDialog dialog(globalPos, this);
            if (dialog.exec() == QDialog::Accepted)
            {
                globalPos = dialog.getGlobalPos();
                scpnt.botPos = dialog.getBotPos();
                scpnt.pnt->SetComponent(new Geom_CartesianPoint(globalPos));
                scpnt.pntLbl->SetPosition(globalPos);
                d_ptr->context->Redisplay(scpnt.pnt, Standard_False);
                d_ptr->context->Redisplay(scpnt.pntLbl, Standard_False);
                d_ptr->viewer->Redraw();
            }
        }
    }
}

void CMainViewport::slRemoveCalibPoint()
{
    const size_t index = static_cast <size_t> (sender()->property("index").toULongLong());
    if (index < d_ptr->calibPoints.size())
    {
        CMainViewportPrivate::SCalibPoint &scpnt = d_ptr->calibPoints[index];
        d_ptr->context->Remove(scpnt.pnt, Standard_False);
        d_ptr->context->Remove(scpnt.pntLbl, Standard_False);
        d_ptr->calibPoints.erase(d_ptr->calibPoints.cbegin() + index);
        for(size_t i = 0; i < d_ptr->calibPoints.size(); ++i)
        {
            const QString txt = QString("  C%1").arg(i + 1);
            d_ptr->calibPoints[i].pntLbl->SetText(txt.toLocal8Bit().constData());
            d_ptr->context->Redisplay(d_ptr->calibPoints[i].pntLbl, Standard_False);
        }
        d_ptr->viewer->Redraw();
    }
}

void CMainViewport::slAddTaskPoint()
{
    Q_ASSERT(sender() != nullptr);

    const GUI_TYPES::TBotTaskType taskType = static_cast <GUI_TYPES::TBotTaskType>
            (sender()->property("taskType").toInt());
    const gp_Pnt point = d_ptr->pnt->Component()->Pnt();
    CBotTaskDialogFacade dialog(this, taskType, point);
    if (dialog.exec() == QDialog::Accepted)
    {
        CMainViewportPrivate::STaskPoint stpnt;
        stpnt.taskType = taskType;
        stpnt.angles = dialog.getAngles();
        const gp_Pnt globalPos = dialog.getPos();
        stpnt.pnt = new AIS_Point(new Geom_CartesianPoint(globalPos));
        stpnt.pntLbl = new AIS_TextLabel();
        stpnt.pntLbl->SetPosition(globalPos);
        const QString txt = QString("  T%1").arg(d_ptr->taskPoints.size() + 1);
        stpnt.pntLbl->SetText(txt.toLocal8Bit().constData());
        d_ptr->taskPoints.push_back(stpnt);
        d_ptr->context->Display(stpnt.pnt, Standard_False);
        d_ptr->context->SetZLayer(stpnt.pntLbl, d_ptr->zLayerId);
        d_ptr->context->Display(stpnt.pntLbl, Standard_False);
        d_ptr->context->Deactivate(stpnt.pntLbl);
        d_ptr->viewer->Redraw();
    }
}

void CMainViewport::slChangeTaskPoint()
{

}

void CMainViewport::slRemoveTaskPoint()
{

}
