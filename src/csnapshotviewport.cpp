#include "csnapshotviewport.h"
#include "ui_csnapshotviewport.h"

#include <QWheelEvent>
#include <QDoubleSpinBox>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_ViewController.hxx>

#include <Image_AlienPixMap.hxx>

#include "cinteractivecontext.h"
#include "caspectwindow.h"

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

    CInteractiveContext *context;
    Handle(V3d_View) view;
    Handle(CAspectWindow) aspect;
    QDoubleSpinBox *scaleSpinbox;
};



static const Quantity_Color BG_CLR   = Quantity_Color( .7765,  .9 , 1.  , Quantity_TOC_RGB);

CSnapshotViewport::CSnapshotViewport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CSnapshotViewport),
    d_ptr(new CSnapshotViewportPrivate())
{
    ui->setupUi(this);

    d_ptr->scaleSpinbox = nullptr;

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    setBackgroundRole(QPalette::NoRole);
    setFocusPolicy(Qt::StrongFocus);
}

CSnapshotViewport::~CSnapshotViewport()
{
    d_ptr->view->Remove();
    delete d_ptr;
    delete ui;
}

void CSnapshotViewport::setContext(CInteractiveContext &context)
{
    d_ptr->context = &context;
    d_ptr->view = d_ptr->context->context().CurrentViewer()->CreateView().get();

    //Aspect
    d_ptr->aspect = new CAspectWindow(*this);
    d_ptr->view->SetWindow(d_ptr->aspect);
    if (!d_ptr->aspect->IsMapped())
        d_ptr->aspect->Map();

    gp_Pnt pos;
    gp_Dir dir;
    Standard_Real len;
    d_ptr->context->getLaserLine(pos, dir, len);
    pos.Translate(len * gp_Vec(dir));
    gp_Pnt lastPoint = pos;
    lastPoint.Translate(len * gp_Vec(dir));
    d_ptr->view->SetEye(pos.X(), pos.Y(), pos.Z());
    d_ptr->view->SetAt(lastPoint.X(), lastPoint.Y(), lastPoint.Z());
    d_ptr->view->SetUp(V3d_Ypos);

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

void CSnapshotViewport::createSnapshot(const char *fname)
{
    Image_PixMap pix;
    V3d_ImageDumpOptions params;
    params.Width = width();
    params.Height = height();
    d_ptr->view->ToPixMap(pix, params);
    Image_AlienPixMap bmpImg;
    bmpImg.InitCopy(pix);
    bmpImg.Save(fname);
}

void CSnapshotViewport::setScale(const double scale)
{
    if (scale > 0)
    {
        d_ptr->view->SetScale(scale);
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
    d_ptr->FlushViewEvents(&d_ptr->context->context(), d_ptr->view, Standard_True);
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
