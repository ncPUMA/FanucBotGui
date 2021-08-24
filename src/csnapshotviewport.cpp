#include "csnapshotviewport.h"
#include "ui_csnapshotviewport.h"

#include <QWheelEvent>
#include <QDoubleSpinBox>
#include <QImage>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_ViewController.hxx>

//#include <Image_AlienPixMap.hxx>

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



static const Quantity_Color BG_CLR   = Quantity_Color(1., 1., 1., Quantity_TOC_RGB);
static const Quantity_Color FACE_CLR = Quantity_Color(0., 0., 0., Quantity_TOC_RGB);

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
    AIS_InteractiveObject &ais_part = d_ptr->context->getAisPart();
    d_ptr->context->context().UnsetLocalAttributes(&ais_part, Standard_False);
    d_ptr->context->context().Redisplay(&ais_part, Standard_True);

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

    //Final
    d_ptr->view->ChangeRenderingParams().IsAntialiasingEnabled = Standard_True;
    d_ptr->view->SetBackgroundColor(BG_CLR);
    d_ptr->view->MustBeResized();

    AIS_InteractiveObject &ais_part = d_ptr->context->getAisPart();
    Handle(Prs3d_Drawer) drawer = new Prs3d_Drawer();
    Handle(Prs3d_ShadingAspect) aShAspect = drawer->ShadingAspect();
    aShAspect->SetColor(BG_CLR);
    drawer->SetShadingAspect(aShAspect);
    drawer->SetShadingModel(Graphic3d_TOSM_UNLIT);

    Handle(Prs3d_LineAspect) lAspect = drawer->FaceBoundaryAspect();
    lAspect->SetColor(FACE_CLR);
    drawer->SetFaceBoundaryAspect(lAspect);
    drawer->SetFaceBoundaryDraw(Standard_True);
    d_ptr->context->context().SetLocalAttributes(&ais_part, drawer, Standard_False);
    d_ptr->context->context().Redisplay(&ais_part, Standard_True);
}

void CSnapshotViewport::setScaleWidget(QDoubleSpinBox &box)
{
    d_ptr->scaleSpinbox = &box;
    d_ptr->updateSpinBox(getScale());
    connect(d_ptr->scaleSpinbox, SIGNAL(valueChanged(double)),
                                 SLOT(slSpinChanged(double)));
}

void CSnapshotViewport::createSnapshot(const char *fname, const size_t width, const size_t height)
{
    Image_PixMap pix;
    V3d_ImageDumpOptions params;
    params.Width = width;
    params.Height = height;
    d_ptr->view->ToPixMap(pix, params);

//    Not working on Windows (possibly due to RGB24 format problem?):
//    Image_AlienPixMap bmpImg;
//    bmpImg.InitCopy(pix);
//    bmpImg.Save(fname);

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
