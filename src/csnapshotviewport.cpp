#include "csnapshotviewport.h"

#include <QWheelEvent>
#include <QDoubleSpinBox>
#include <QImage>

#include <V3d_BadValue.hxx>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_ViewController.hxx>
#include <gp_Quaternion.hxx>

#include "cinteractivecontext.h"
#include "caspectwindow.h"

static const Quantity_Color BG_CLR   = Quantity_Color(1., 1., 1., Quantity_TOC_RGB);
static const Quantity_Color FACE_CLR = Quantity_Color(0., 0., 0., Quantity_TOC_RGB);

class CSnapshotV3dView : public V3d_View
{
public:
    CSnapshotV3dView(CInteractiveContext &cntxt, const V3d_TypeOfView theType = V3d_ORTHOGRAPHIC) :
        V3d_View(cntxt.context().CurrentViewer(), theType),
        context(&cntxt) {
        drawer = new Prs3d_Drawer();
        Handle(Prs3d_ShadingAspect) aShAspect = drawer->ShadingAspect();
        aShAspect->SetColor(BG_CLR);
        drawer->SetShadingAspect(aShAspect);
        drawer->SetShadingModel(Graphic3d_TOSM_UNLIT);

        Handle(Prs3d_LineAspect) lAspect = drawer->FaceBoundaryAspect();
        lAspect->SetColor(FACE_CLR);
        drawer->SetFaceBoundaryAspect(lAspect);
        drawer->SetFaceBoundaryDraw(Standard_True);

        origDrawer = context->context().DefaultDrawer();
    }

    void beforeRedraw() const {
        context->hideAllAdditionalObjects();
        AIS_InteractiveObject &ais_part = context->getAisPart();
        context->context().SetLocalAttributes(&ais_part, drawer, Standard_False);
        context->context().Redisplay(&ais_part, Standard_False);
    }

    void afterRedraw() const {
        context->showAllAdditionalObjects();
        AIS_InteractiveObject &ais_part = context->getAisPart();
        context->context().SetLocalAttributes(&ais_part, origDrawer, Standard_False);
        context->context().Redisplay(&ais_part, Standard_False);
    }

    void update() const {
        if (!myView->IsDefined()
         || !myView->IsActive()) {
          return;
        }

        myIsInvalidatedImmediate = Standard_False;
        myView->Update();
        myView->Compute();
        AutoZFit();
        Redraw();
    }

    void setScale(const Standard_Real Coef) {
        V3d_BadValue_Raise_if( Coef <= 0. ,"CSnapshotV3dView::setScale, bad coefficient");

        Handle(Graphic3d_Camera) aCamera = Camera();

        Standard_Real aDefaultScale = myDefaultCamera->Scale();
        aCamera->SetAspect (myDefaultCamera->Aspect());
        aCamera->SetScale (aDefaultScale / Coef);

        update();
    }

    void Redraw() const {
        beforeRedraw();
        V3d_View::Redraw();
        afterRedraw();
    }

    void RedrawImmediate() const {
        beforeRedraw();
        V3d_View::RedrawImmediate();
        afterRedraw();
    }

    void updatePosition() {
        gp_Pnt pos;
        gp_Dir dir;
        Standard_Real len;
        context->getLaserLine(pos, dir, len);

        /**
         *  TODO: parametrize camera distance (e.g. 80 mm)
         *        (plays a role when we look towards physycally-inspired
         *                                     rendering and perspective)
         */
        Standard_Real len_cam = 0;

        pos.Translate(-len_cam * gp_Vec(dir));
        SetEye(pos.X(), pos.Y(), pos.Z());
        pos.Translate(len * gp_Vec(dir));
        gp_Pnt lastPoint = pos;
        SetAt(lastPoint.X(), lastPoint.Y(), lastPoint.Z());

        const gp_Trsf trsf = context->getTransform(GUI_TYPES::ENST_LSRHEAD);
        const gp_Quaternion rotation = trsf.GetRotation();
        gp_Vec orient(1, 0, 0);
        const gp_Vec orient_rot = rotation.Multiply(orient);
        SetUp(orient_rot.X(), orient_rot.Y(), orient_rot.Z());

        Redraw();
    }

    CInteractiveContext& getContext() const { return *context; }

private:
    CInteractiveContext * const context;
    Handle(Prs3d_Drawer) drawer;
    Handle(Prs3d_Drawer) origDrawer;
};



class CSnapshotViewportPrivate : public AIS_ViewController
{
    friend class CSnapshotViewport;

    void updateSpinBox(const double scale) {
        if (scaleSpinbox) {
            scaleSpinbox->blockSignals(true);
            scaleSpinbox->setValue(scale);
            scaleSpinbox->blockSignals(false);
        }
    }

    Handle(CSnapshotV3dView) view;
    Handle(CAspectWindow) aspect;
    QDoubleSpinBox *scaleSpinbox;
};



CSnapshotViewport::CSnapshotViewport(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CSnapshotViewportPrivate())
{
    d_ptr->scaleSpinbox = nullptr;

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    setBackgroundRole(QPalette::NoRole);
    setFocusPolicy(Qt::StrongFocus);
}

CSnapshotViewport::~CSnapshotViewport()
{
    delete d_ptr;
}

void CSnapshotViewport::setContext(CInteractiveContext &context)
{
    d_ptr->view = new CSnapshotV3dView(context, V3d_ORTHOGRAPHIC);

    //Aspect
    d_ptr->aspect = new CAspectWindow(*this);
    d_ptr->view->SetWindow(d_ptr->aspect);
    if (!d_ptr->aspect->IsMapped())
        d_ptr->aspect->Map();

    //Final
    d_ptr->view->ChangeRenderingParams().IsAntialiasingEnabled = Standard_True;
    d_ptr->view->SetBackgroundColor(BG_CLR);
    d_ptr->view->MustBeResized();
}

void CSnapshotViewport::setScaleWidget(QDoubleSpinBox &box)
{
    d_ptr->scaleSpinbox = &box;
    d_ptr->updateSpinBox(getScale());
    connect(d_ptr->scaleSpinbox, SIGNAL(valueChanged(double)),
            SLOT(slSpinChanged(double)));
}

void CSnapshotViewport::updatePosition()
{
    d_ptr->view->updatePosition();
}

void CSnapshotViewport::createSnapshot(const char *fname, const size_t width, const size_t height)
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

void CSnapshotViewport::setScale(const double scale)
{
    if (scale > 0)
    {
        d_ptr->view->setScale(scale);
        d_ptr->updateSpinBox(scale);
    }
}

double CSnapshotViewport::getScale() const
{
    double result = 0.;
    if (d_ptr->view)
        result = d_ptr->view->Scale();
    return result;
}

QPaintEngine *CSnapshotViewport::paintEngine() const
{
    return nullptr;
}

void CSnapshotViewport::paintEvent(QPaintEvent *)
{
    d_ptr->view->InvalidateImmediate();
    d_ptr->FlushViewEvents(&d_ptr->view->getContext().context(), d_ptr->view, Standard_True);
}

void CSnapshotViewport::resizeEvent(QResizeEvent *)
{
    d_ptr->view->MustBeResized();
}

////! Map Qt buttons bitmask to virtual keys.
//inline static Aspect_VKeyMouse qtMouseButtons2VKeys(Qt::MouseButtons theButtons)
//{
//    Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
//    if ((theButtons & Qt::LeftButton) != 0)
//    {
//        aButtons |= Aspect_VKeyMouse_LeftButton;
//    }
//    if ((theButtons & Qt::MiddleButton) != 0)
//    {
//        aButtons |= Aspect_VKeyMouse_MiddleButton;
//    }
//    if ((theButtons & Qt::RightButton) != 0)
//    {
//        aButtons |= Aspect_VKeyMouse_RightButton;
//    }
//    return aButtons;
//}

////! Map Qt mouse modifiers bitmask to virtual keys.
//inline static Aspect_VKeyFlags qtMouseModifiers2VKeys(Qt::KeyboardModifiers theModifiers)
//{
//    Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
//    if ((theModifiers & Qt::ShiftModifier) != 0)
//    {
//        aFlags |= Aspect_VKeyFlags_SHIFT;
//    }
//    if ((theModifiers & Qt::ControlModifier) != 0)
//    {
//        aFlags |= Aspect_VKeyFlags_CTRL;
//    }
//    if ((theModifiers & Qt::AltModifier) != 0)
//    {
//        aFlags |= Aspect_VKeyFlags_ALT;
//    }
//    return aFlags;
//}

//void CSnapshotViewport::mousePressEvent(QMouseEvent *event)
//{
//    const Graphic3d_Vec2i aPnt(event->pos().x(), event->pos().y());
//    const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(event->modifiers());
//    if (d_ptr->UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(event->buttons()), aFlags, false))
//        update();
//}

//void CSnapshotViewport::mouseReleaseEvent(QMouseEvent *event)
//{
//    const Graphic3d_Vec2i aPnt(event->pos().x(), event->pos().y());
//    const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(event->modifiers());
//    if (d_ptr->UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(event->buttons()), aFlags, false))
//        update();
//}

//void CSnapshotViewport::mouseMoveEvent(QMouseEvent *event)
//{
//    const Graphic3d_Vec2i aNewPos(event->pos().x(), event->pos().y());
//    if (d_ptr->UpdateMousePosition(aNewPos,
//                                   qtMouseButtons2VKeys(event->buttons()),
//                                   qtMouseModifiers2VKeys(event->modifiers()),
//                                   false))
//    {
//        update();
//    }
//}

void CSnapshotViewport::wheelEvent(QWheelEvent *event)
{
    double delta = 0.1;
    if (event->delta() < 0)
        delta = - 0.1;
    setScale(getScale() + delta);
}

void CSnapshotViewport::slSpinChanged(double value)
{
    setScale(value);
}
