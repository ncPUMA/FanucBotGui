#include "cmainviewport.h"

#include <vector>
#include <map>

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

#include "cinteractivecontext.h"
#include "caspectwindow.h"
#include "sguisettings.h"

#include "Dialogs/caddcalibpointdialog.h"
#include "Dialogs/cbottaskdialogfacade.h"

static constexpr double DEGREE_K = M_PI / 180.;

static const Quantity_Color BG_CLR   = Quantity_Color( .7765,  .9 , 1.  , Quantity_TOC_RGB);
static const Graphic3d_ZLayerId Z_LAYER_ID_WITHOUT_DEPTH_TEST_DEFAULT = 100;

class CMainViewportPrivate : public AIS_ViewController
{
    friend class CMainViewport;

    CMainViewportPrivate(CMainViewport * const qptr) :
        q_ptr(qptr),
        context(new CInteractiveContext()),
        usrAction(GUI_TYPES::ENUA_NO) {
        myMouseGestureMap.Clear();
        myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Pan);
        myMouseGestureMap.Bind(Aspect_VKeyMouse_RightButton, AIS_MouseGesture_RotateOrbit);
        SetAllowRotation(Standard_True);
    }

    ~CMainViewportPrivate() {
        delete context;
    }

    void init(OpenGl_GraphicDriver &driver) {
        //Viewer
        viewer = new V3d_Viewer(&driver);
        viewer->SetDefaultViewSize(1000.);
        viewer->SetComputedMode(Standard_True);
        viewer->SetDefaultComputedMode(Standard_True);
        viewer->SetDefaultLights();
        viewer->SetLightOn();

        //ZLayer without depth-test
        Graphic3d_ZLayerId zLayerIdWithoutDepthTest = Graphic3d_ZLayerId_UNKNOWN;
        viewer->AddZLayer(zLayerIdWithoutDepthTest);
        Graphic3d_ZLayerSettings zSettings = viewer->ZLayerSettings(zLayerIdWithoutDepthTest);
        zSettings.SetEnableDepthTest(Standard_False);
        viewer->SetZLayerSettings(zLayerIdWithoutDepthTest, zSettings);

        //Context
        AIS_InteractiveContext * const cntxt = new AIS_InteractiveContext(viewer);
        context->setDisableTepthTestZLayer(zLayerIdWithoutDepthTest);
        context->init(*cntxt);
        view = cntxt->CurrentViewer()->CreateView().get();

        //Aspect
        aspect = new CAspectWindow(*q_ptr);
        view->SetWindow(aspect);
        if (!aspect->IsMapped())
            aspect->Map();

        //Final
        view->ChangeRenderingParams().IsAntialiasingEnabled = Standard_True;
        view->SetBackgroundColor(BG_CLR);
        view->MustBeResized();
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

        return trsfTr3 * trsfSc * trsfTr2 * trsfRcur * trsfRoff * trsfTr1;
    }

    gp_Trsf calcPartTrsf() const {
        return calc_transform(gp_Vec(guiSettings.partTrX,
                                     guiSettings.partTrY,
                                     guiSettings.partTrZ),
                              gp_Vec(guiSettings.partCenterX,
                                     guiSettings.partCenterY,
                                     guiSettings.partCenterZ),
                              guiSettings.partScale,
                              guiSettings.partRotationX,
                              guiSettings.partRotationY,
                              guiSettings.partRotationZ);
    }

    gp_Trsf calcDeskTrsf() const {
        return calc_transform(gp_Vec(guiSettings.deskTrX,
                                     guiSettings.deskTrY,
                                     guiSettings.deskTrZ),
                              gp_Vec(guiSettings.deskCenterX,
                                     guiSettings.deskCenterY,
                                     guiSettings.deskCenterZ),
                              guiSettings.deskScale,
                              guiSettings.deskRotationX,
                              guiSettings.deskRotationY,
                              guiSettings.deskRotationZ);
    }

    gp_Trsf calcLsrheadTrsf() const { return gp_Trsf(); }

    gp_Trsf calcGripTrsf() const {
        return calc_transform(gp_Vec(guiSettings.gripTrX,
                                     guiSettings.gripTrY,
                                     guiSettings.gripTrZ),
                              gp_Vec(guiSettings.gripCenterX,
                                     guiSettings.gripCenterY,
                                     guiSettings.gripCenterZ),
                              guiSettings.gripScale,
                              guiSettings.gripRotationX,
                              guiSettings.gripRotationY,
                              guiSettings.gripRotationZ);
    }

    void setGuiSettings(const SGuiSettings &settings) {
        guiSettings = settings;
        view->ChangeRenderingParams().NbMsaaSamples = settings.msaa;
        context->setPartMdlTransform(calcPartTrsf());
        context->setDeskMdlTransform(calcDeskTrsf());
        context->setGripMdlTransform(calcGripTrsf());
        view->Redraw();
    }

    void setPartModel(const TopoDS_Shape &shape) {
        context->setPartModel(shape);
        context->setPartMdlTransform(calcPartTrsf());
        view->Redraw();
    }

    void setDeskModel(const TopoDS_Shape &shape) {
        context->setDeskModel(shape);
        context->setDeskMdlTransform(calcDeskTrsf());
        view->Redraw();
    }

    void setLsrheadModel(const TopoDS_Shape &shape) {
        context->setLsrheadModel(shape);
        context->setLsrheadMdlTransform(calcLsrheadTrsf());
        view->Redraw();
    }

    void setGripModel(const TopoDS_Shape &shape) {
        context->setGripModel(shape);
        context->setGripMdlTransform(calcGripTrsf());
        view->Redraw();
    }

    void setMSAA(const GUI_TYPES::TMSAA msaa) {
        assert(!view.IsNull());
        guiSettings.msaa = msaa;
        view->ChangeRenderingParams().NbMsaaSamples = msaa;
        view->Redraw();
    }

    void setShading(const bool enabled) {
        context->setShading(enabled);
        view->Redraw();
    }

    void setUserAction(const GUI_TYPES::EN_UserActions userAction) {
        usrAction = userAction;
        context->hideAllAdditionalObjects();
        switch(userAction) {
            case GUI_TYPES::ENUA_CALIBRATION: context->showCalibObjects(); break;
            case GUI_TYPES::ENUA_ADD_TASK   : context->showTaskObjects();  break;
            default: break;
        }
        view->Redraw();
    }


    CMainViewport * const q_ptr;

    Handle(V3d_Viewer)             viewer;
    Handle(V3d_View)               view;
    Handle(CAspectWindow)          aspect;

    SGuiSettings guiSettings;
    CInteractiveContext * const context;

    GUI_TYPES::EN_UserActions usrAction;
    QPoint rbPos;
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
    d_ptr->setGuiSettings(settings);
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
    return static_cast <GUI_TYPES::TMSAA> (d_ptr->view->RenderingParams().NbMsaaSamples);
}

void CMainViewport::setStatsVisible(const bool value)
{
    d_ptr->view->ChangeRenderingParams().ToShowStats = value;
}

void CMainViewport::setShading(const bool enabled)
{
    d_ptr->setShading(enabled);
}

void CMainViewport::fitInView()
{
    d_ptr->view->FitAll();
    d_ptr->view->ZFitAll();
    d_ptr->view->Redraw();
}

void CMainViewport::setCoord(const GUI_TYPES::TCoordSystem type)
{
    V3d_TypeOfOrientation orientation = V3d_XposYnegZpos;
    if (type == GUI_TYPES::ENCS_LEFT)
        orientation = V3d_XposYnegZneg;
    d_ptr->view->SetProj(orientation, Standard_False);
    d_ptr->view->Redraw();
}

void CMainViewport::setUserAction(const GUI_TYPES::EN_UserActions usrAction)
{
    d_ptr->setUserAction(usrAction);
}

GUI_TYPES::EN_UserActions CMainViewport::getUsrAction() const
{
    return d_ptr->usrAction;
}

void CMainViewport::setPartModel(const TopoDS_Shape &shape)
{
    d_ptr->setPartModel(shape);
}

void CMainViewport::setDeskModel(const TopoDS_Shape &shape)
{
    d_ptr->setDeskModel(shape);
}

void CMainViewport::setLsrheadModel(const TopoDS_Shape &shape)
{
    d_ptr->setLsrheadModel(shape);
}

void CMainViewport::setGripModel(const TopoDS_Shape &shape)
{
    d_ptr->setGripModel(shape);
}

const TopoDS_Shape& CMainViewport::getPartShape() const
{
    return d_ptr->context->getPartShape();
}

const TopoDS_Shape& CMainViewport::getDeskShape() const
{
    return d_ptr->context->getDeskShape();
}

const TopoDS_Shape& CMainViewport::getLsrheadShape() const
{
    return d_ptr->context->getLsrHeadShape();
}

const TopoDS_Shape& CMainViewport::getGripShape() const
{
    return d_ptr->context->getGripShape();
}

void CMainViewport::moveLsrhead(const GUI_TYPES::SVertex &pos, const GUI_TYPES::SRotationAngle &angle)
{

}

void CMainViewport::moveGrip(const GUI_TYPES::SVertex &pos, const GUI_TYPES::SRotationAngle &angle)
{

}

QPaintEngine *CMainViewport::paintEngine() const
{
    return nullptr;
}

void CMainViewport::paintEvent(QPaintEvent *)
{
    d_ptr->view->InvalidateImmediate();
    d_ptr->FlushViewEvents(&d_ptr->context->context(), d_ptr->view, Standard_True);
}

void CMainViewport::resizeEvent(QResizeEvent *)
{
    d_ptr->view->MustBeResized();
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
                fillCalibCntxtMenu(menu);
                break;

            case ENUA_ADD_TASK:
                fillTaskAddCntxtMenu(menu);
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
    d_ptr->context->updateCursorPosition();
    d_ptr->view->Redraw();
}

void CMainViewport::wheelEvent(QWheelEvent *event)
{
    const Graphic3d_Vec2i aPos(event->pos().x(), event->pos().y());
    if (d_ptr->UpdateZoom(Aspect_ScrollDelta(aPos, event->delta() / 8)))
        update();
}

QString CMainViewport::taskName(const GUI_TYPES::TBotTaskType taskType) const
{
    using namespace GUI_TYPES;
    const std::map <TBotTaskType, QString> mapNames = {
        { ENBTT_MOVE , tr("Перемещение") },
        { ENBTT_DRILL, tr("Сверление")   },
        { ENBTT_MARK , tr("Маркировка")  }
    };
    return extract_map_value(mapNames, taskType, QString());
}

void CMainViewport::fillCalibCntxtMenu(QMenu &menu)
{
    if (d_ptr->context->isPartDetected())
    {
        menu.addAction(CMainViewport::tr("Точка привязки"),
                       this,
                       &CMainViewport::slAddCalibPoint);
    }
    else
    {
        size_t index = 0;
        if (d_ptr->context->isCalibPointDetected(index))
        {
            menu.addAction(CMainViewport::tr("Изменить"),
                           this,
                           &CMainViewport::slChangeCalibPoint)->
                    setProperty("index", static_cast <qulonglong> (index));
            menu.addAction(CMainViewport::tr("Удалить"),
                           this,
                           &CMainViewport::slRemoveCalibPoint)->
                    setProperty("index", static_cast <qulonglong> (index));;
        }
    }
}

void CMainViewport::fillTaskAddCntxtMenu(QMenu &menu)
{
    if (d_ptr->context->isPartDetected())
    {
        using namespace GUI_TYPES;
        menu.addAction(taskName(ENBTT_MOVE),
                       this,
                       &CMainViewport::slAddTaskPoint)->setProperty("taskType", ENBTT_MOVE);
        menu.addAction(taskName(ENBTT_DRILL),
                       this,
                       &CMainViewport::slAddTaskPoint)->setProperty("taskType", ENBTT_DRILL);;
        menu.addAction(taskName(ENBTT_MARK),
                       this,
                       &CMainViewport::slAddTaskPoint)->setProperty("taskType", ENBTT_MARK);;
    }
    else
    {
        size_t index = 0;
        if (d_ptr->context->isTaskPointDetected(index))
        {
                menu.addAction(CMainViewport::tr("Изменить"),
                               this,
                               &CMainViewport::slChangeTaskPoint)->
                        setProperty("index", static_cast <qulonglong> (index));
                menu.addAction(CMainViewport::tr("Удалить"),
                               this,
                               &CMainViewport::slRemoveTaskPoint)->
                        setProperty("index", static_cast <qulonglong> (index));;
        }
    }
}

void CMainViewport::slAddCalibPoint()
{
    const gp_Pnt cursorPos = d_ptr->context->lastCursorPosition();
    GUI_TYPES::SCalibPoint initPoint;
    initPoint.globalPos.x = cursorPos.X();
    initPoint.globalPos.y = cursorPos.Y();
    initPoint.globalPos.z = cursorPos.Z();
    CAddCalibPointDialog dialog(this, initPoint);
    if (dialog.exec() == QDialog::Accepted)
    {
        d_ptr->context->appendCalibPoint(dialog.getCalibPoint());
        d_ptr->viewer->Redraw();
    }
}

void CMainViewport::slChangeCalibPoint()
{
    assert(sender() != nullptr);

    const size_t index = static_cast <size_t> (sender()->property("index").toULongLong());
    if (index < d_ptr->context->getCalibPointCount())
    {
        const GUI_TYPES::SCalibPoint calibPnt = d_ptr->context->getCalibPoint(index);
        CAddCalibPointDialog dialog(this, calibPnt);
        if (dialog.exec() == QDialog::Accepted)
        {
            d_ptr->context->changeCalibPoint(index, dialog.getCalibPoint());
            d_ptr->viewer->Redraw();
        }
    }
}

void CMainViewport::slRemoveCalibPoint()
{
    assert(sender() != nullptr);

    const size_t index = static_cast <size_t> (sender()->property("index").toULongLong());
    if (index < d_ptr->context->getCalibPointCount())
    {
        d_ptr->context->removeCalibPoint(index);
        d_ptr->viewer->Redraw();
    }
}

void CMainViewport::slAddTaskPoint()
{
    assert(sender() != nullptr);

    GUI_TYPES::STaskPoint taskPoint;
    taskPoint.taskType = static_cast <GUI_TYPES::TBotTaskType>
            (sender()->property("taskType").toInt());
    const gp_Pnt cursorPos = d_ptr->context->lastCursorPosition();
    taskPoint.globalPos.x = cursorPos.X();
    taskPoint.globalPos.y = cursorPos.Y();
    taskPoint.globalPos.z = cursorPos.Z();

    CBotTaskDialogFacade dialog(this, taskPoint);
    if (dialog.exec() == QDialog::Accepted)
    {
        d_ptr->context->appendTaskPoint(dialog.getTaskPoint());
        d_ptr->viewer->Redraw();
    }
}

void CMainViewport::slChangeTaskPoint()
{
    assert(sender() != nullptr);

    const size_t index = static_cast <size_t> (sender()->property("index").toULongLong());
    if (index < d_ptr->context->getTaskPointCount())
    {
        const GUI_TYPES::STaskPoint taskPoint = d_ptr->context->getTaskPoint(index);
        CBotTaskDialogFacade dialog(this, taskPoint);
        if (dialog.exec() == QDialog::Accepted)
        {
            d_ptr->context->changeTaskPoint(index, dialog.getTaskPoint());
            d_ptr->viewer->Redraw();
        }
    }
}

void CMainViewport::slRemoveTaskPoint()
{
    assert(sender() != nullptr);

    const size_t index = static_cast <size_t> (sender()->property("index").toULongLong());
    if (index < d_ptr->context->getTaskPointCount())
    {
        d_ptr->context->removeTaskPoint(index);
        d_ptr->viewer->Redraw();
    }
}
