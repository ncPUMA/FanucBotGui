#include "cadvancedviewport.h"

#include <QWheelEvent>
#include <QDoubleSpinBox>
#include <QImage>

#include <V3d_BadValue.hxx>

#include <OpenGl_GraphicDriver.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_ViewController.hxx>
#include <gp_Quaternion.hxx>
#include <AIS_Shape.hxx>

#include "caspectwindow.h"

static const Quantity_Color BG_CLR   = Quantity_Color(1., 1., 1., Quantity_TOC_RGB);

class CAdvancedViewportPrivate : public AIS_ViewController
{
    friend class CAdvancedViewport;

    void init(OpenGl_GraphicDriver &driver, CAdvancedViewport &qptr) {
        //Viewer
        viewer = new V3d_Viewer(&driver);
        viewer->SetDefaultTypeOfView(V3d_ORTHOGRAPHIC);
        viewer->SetDefaultViewSize(1000.);
        viewer->SetDefaultViewProj(V3d_XposYposZpos);
        viewer->SetComputedMode(Standard_True);
        viewer->SetDefaultComputedMode(Standard_True);
        viewer->SetDefaultLights();
        viewer->SetLightOn();

        //Context
        context = new AIS_InteractiveContext(viewer);
        view = context->CurrentViewer()->CreateView().get();

        //Aspect
        aspect = new CAspectWindow(qptr);
        view->SetWindow(aspect);
        if (!aspect->IsMapped())
            aspect->Map();

        //Final
        view->ChangeRenderingParams().NbMsaaSamples = 8;
        view->ChangeRenderingParams().IsAntialiasingEnabled = Standard_True;
        view->SetBackgroundColor(BG_CLR);
        view->MustBeResized();
    }

    bool modelShapeChanged(const GUI_TYPES::EN_ShapeType model,
                           const TopoDS_Shape &shape) {
        bool needUpdate = false;
        switch(model)
        {
            using namespace GUI_TYPES;

            case ENST_DESK:
                context->Remove(ais_desk, Standard_False);
                ais_desk = new AIS_Shape(shape);
                context->SetDisplayMode(ais_desk, AIS_Shaded, Standard_False);
                context->Display(ais_desk, Standard_False);
                context->Deactivate(ais_desk);
                needUpdate = true;
                break;
            case ENST_PART:
                context->Remove(ais_part, Standard_False);
                ais_part = new AIS_Shape(shape);
                context->SetDisplayMode(ais_part, AIS_Shaded, Standard_False);
                context->Display(ais_part, Standard_False);
                context->Deactivate(ais_part);
                needUpdate = true;
                break;
            case ENST_LSRHEAD:
            case ENST_GRIP:
                break;
        }
        return needUpdate;
    }

    bool modelTransformChanged(const GUI_TYPES::EN_ShapeType model,
                               const gp_Trsf &trsf) {
        bool needUpdate = false;
        switch(model)
        {
            using namespace GUI_TYPES;

            case ENST_DESK:
                context->SetLocation(ais_desk, trsf);
                needUpdate = true;
                break;
            case ENST_PART:
                context->SetLocation(ais_part, trsf);
                needUpdate = true;
                break;
            case ENST_LSRHEAD:
            case ENST_GRIP:
                break;
        }
        return needUpdate;
    }

    void setShapeVisible(const GUI_TYPES::EN_ShapeType model, bool visible) {
        bool needUpdate = false;
        Handle(AIS_InteractiveObject ) obj;
        switch(model)
        {
            using namespace GUI_TYPES;

            case ENST_DESK:
                obj = ais_desk;
                break;
            case ENST_PART:
                obj = ais_part;
                break;
            case ENST_LSRHEAD:
            case ENST_GRIP:
                break;
        }

        if (!obj.IsNull())
        {
            bool isVis = context->IsDisplayed(obj);
            if (isVis && !visible)
            {
                context->Erase(obj, Standard_False);
                needUpdate = true;
            }
            else if (!isVis && visible)
            {
                context->SetDisplayMode(obj, AIS_Shaded, Standard_False);
                context->Display(obj, Standard_False);
                context->Deactivate(obj);
                needUpdate = true;
            }
        }

        if (needUpdate)
            view->Redraw();
    }

    Handle(V3d_Viewer) viewer;
    Handle(V3d_View) view;
    Handle(CAspectWindow) aspect;
    Handle(AIS_InteractiveContext) context;

    gp_Pnt laserPos;
    gp_Dir laserDir;

    Handle(AIS_Shape) ais_desk;
    Handle(AIS_Shape) ais_part;
};



CAdvancedViewport::CAdvancedViewport(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new CAdvancedViewportPrivate())
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    setBackgroundRole(QPalette::NoRole);
    setFocusPolicy(Qt::StrongFocus);
}

CAdvancedViewport::~CAdvancedViewport()
{
    delete d_ptr;
}

void CAdvancedViewport::init(OpenGl_GraphicDriver &driver)
{
    d_ptr->init(driver, *this);
    initPrivate(*d_ptr->context);
}

void CAdvancedViewport::modelShapeChanged(const GUI_TYPES::EN_ShapeType model,
                                          const TopoDS_Shape &shape)
{
    bool needRedraw = d_ptr->modelShapeChanged(model, shape);
    switch(model)
    {
        using namespace GUI_TYPES;

        case ENST_DESK:
            needRedraw |= modelShapeChangedPrivate(*d_ptr->context, model, *d_ptr->ais_desk);
            break;
        case ENST_PART:
            needRedraw |= modelShapeChangedPrivate(*d_ptr->context, model, *d_ptr->ais_part);
            break;
        case ENST_LSRHEAD:
        case ENST_GRIP:
            break;
    }

    if (needRedraw)
        d_ptr->view->Redraw();
}

void CAdvancedViewport::modelTransformChanged(const GUI_TYPES::EN_ShapeType model,
                                              const gp_Trsf &trsf)
{
    bool needRedraw = false;
    if (model == GUI_TYPES::ENST_LSRHEAD)
    {
        gp_Pnt pos = d_ptr->laserPos;
        pos.Transform(trsf);
        d_ptr->view->SetEye(pos.X(), pos.Y(), pos.Z());

        gp_Dir dir = d_ptr->laserDir;
        dir.Transform(trsf);

        gp_Pnt aLastPoint = pos;
        aLastPoint.Translate(gp_Vec(dir));
        d_ptr->view->SetAt(aLastPoint.X(), aLastPoint.Y(), aLastPoint.Z());

        /**
         *  TODO: parametrize camera distance (e.g. 80 mm)
         *        (plays a role when we look towards physycally-inspired
         *                                     rendering and perspective)
         */
        Standard_Real len_cam = 0;
        pos.Translate(-len_cam * gp_Vec(dir));
        const gp_Quaternion rotation = trsf.GetRotation();
        gp_Vec orient(1, 0, 0);
        const gp_Vec orient_rot = rotation.Multiply(orient);
        d_ptr->view->SetUp(orient_rot.X(), orient_rot.Y(), orient_rot.Z());

        needRedraw = true;
    }

    needRedraw |= d_ptr->modelTransformChanged(model, trsf)
            || modelTransformChangedPrivate(*d_ptr->context, model, trsf);

    if (needRedraw)
        d_ptr->view->Redraw();
}

void CAdvancedViewport::setCameraScale(const double scale)
{
    d_ptr->view->SetScale(scale);
}

void CAdvancedViewport::setCameraPos(const gp_Pnt &pos, const gp_Dir &dir, const gp_Dir &orient)
{
    d_ptr->view->SetEye(pos.X(), pos.Y(), pos.Z());
    d_ptr->view->SetAt(dir.X(), dir.Y(), dir.Z());
    d_ptr->view->SetUp(orient.X(), orient.Y(), orient.Z());
    d_ptr->view->Redraw();
}

void CAdvancedViewport::setLaserPos(const gp_Pnt &pos, const gp_Dir &dir)
{
    d_ptr->laserPos = pos;
    d_ptr->laserDir = dir;
}

void CAdvancedViewport::createSnapshot(const char *fname, const size_t width, const size_t height)
{
    Image_PixMap pix;
    V3d_ImageDumpOptions params;
    params.Width = width;
    params.Height = height;
    d_ptr->view->ToPixMap(pix, params);

    QImage img(pix.Width(), pix.Height(), QImage::Format_RGB32);
    for (Standard_Size y = 0; y < pix.Height(); y++)
    {
        uchar * const pRowSrc = pix.ChangeRow(y);
        uchar * const pRowDest = img.scanLine(y);
        for (Standard_Size x = 0; x < pix.Width(); x++)
        {
            pRowDest[x * 4    ] = pRowSrc[x * 3 + 2];
            pRowDest[x * 4 + 1] = pRowSrc[x * 3 + 1];
            pRowDest[x * 4 + 2] = pRowSrc[x * 3 + 0];
            pRowDest[x * 4 + 3] = 0;
        }
    }
    img.save(fname);
}

QImage CAdvancedViewport::createSnapshot(const size_t width, const size_t height)
{
    Image_PixMap pix;
    V3d_ImageDumpOptions params;
    params.Width = width;
    params.Height = height;
    d_ptr->view->ToPixMap(pix, params);

    QImage img(pix.Width(), pix.Height(), QImage::Format_RGB32);
    for (Standard_Size y = 0; y < pix.Height(); y++)
    {
        uchar * const pRowSrc = pix.ChangeRow(y);
        uchar * const pRowDest = img.scanLine(y);
        for (Standard_Size x = 0; x < pix.Width(); x++)
        {
            pRowDest[x * 4    ] = pRowSrc[x * 3 + 2];
            pRowDest[x * 4 + 1] = pRowSrc[x * 3 + 1];
            pRowDest[x * 4 + 2] = pRowSrc[x * 3 + 0];
            pRowDest[x * 4 + 3] = 0;
        }
    }
    return img;
}

void CAdvancedViewport::setShapeVisible(const GUI_TYPES::EN_ShapeType model, bool visible)
{
    d_ptr->setShapeVisible(model, visible);
}

QPaintEngine *CAdvancedViewport::paintEngine() const
{
    return nullptr;
}

void CAdvancedViewport::paintEvent(QPaintEvent *)
{
    d_ptr->view->InvalidateImmediate();
    d_ptr->FlushViewEvents(d_ptr->context, d_ptr->view, Standard_True);
}

void CAdvancedViewport::resizeEvent(QResizeEvent *)
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

void CAdvancedViewport::mousePressEvent(QMouseEvent *event)
{
    const Graphic3d_Vec2i aPnt(event->pos().x(), event->pos().y());
    const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(event->modifiers());
    if (d_ptr->UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(event->buttons()), aFlags, false))
        update();
}

void CAdvancedViewport::mouseReleaseEvent(QMouseEvent *event)
{
    const Graphic3d_Vec2i aPnt(event->pos().x(), event->pos().y());
    const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(event->modifiers());
    if (d_ptr->UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(event->buttons()), aFlags, false))
        update();
}

void CAdvancedViewport::mouseMoveEvent(QMouseEvent *event)
{
    const Graphic3d_Vec2i aNewPos(event->pos().x(), event->pos().y());
    if (d_ptr->UpdateMousePosition(aNewPos,
                                   qtMouseButtons2VKeys(event->buttons()),
                                   qtMouseModifiers2VKeys(event->modifiers()),
                                   false))
    {
        update();
    }
}

void CAdvancedViewport::wheelEvent(QWheelEvent *event)
{
    double delta = 0.1;
    if (event->delta() < 0)
        delta = - 0.1;
    setCameraScale(d_ptr->view->Scale() + delta);
    update();
}
