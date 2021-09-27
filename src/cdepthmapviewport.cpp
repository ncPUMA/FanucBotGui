#include "cdepthmapviewport.h"

#include <QImage>
#include <QFile>
#include <QDebug>

#include <V3d_BadValue.hxx>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_ViewController.hxx>
#include <gp_Quaternion.hxx>
#include <Graphic3d_ShaderProgram.hxx>

#include "cinteractivecontext.h"
#include "caspectwindow.h"

static const Quantity_Color BG_CLR   = Quantity_Color(1., 1., 1., Quantity_TOC_RGB);

class CDepthMapV3dView : public V3d_View
{
public:
    CDepthMapV3dView(CInteractiveContext &cntxt, const V3d_TypeOfView theType = V3d_ORTHOGRAPHIC) :
        V3d_View(cntxt.context().CurrentViewer(), theType),
        context(&cntxt),
        depthMapShader(new Graphic3d_ShaderProgram()),
        origShader(new Graphic3d_ShaderProgram())
    {
        QByteArray vertText;
        QFile vertFile(":/Shaders/Data/Shaders/depthMapShader.vert");
        if (vertFile.open(QIODevice::ReadOnly | QIODevice::Text))
            vertText = vertFile.readAll();
        Handle(Graphic3d_ShaderObject) vertex =
            Graphic3d_ShaderObject::CreateFromSource(Graphic3d_TOS_VERTEX, vertText.constData());

        QByteArray fragText;
        QFile fragFile(":/Shaders/Data/Shaders/depthMapShader.frag");
        if (fragFile.open(QIODevice::ReadOnly | QIODevice::Text))
            fragText = fragFile.readAll();
        Handle(Graphic3d_ShaderObject) fragment =
            Graphic3d_ShaderObject::CreateFromSource(Graphic3d_TOS_FRAGMENT, fragText.constData());

        qDebug() << depthMapShader->AttachShader(vertex);
        qDebug() << depthMapShader->AttachShader(fragment);
    }

    void beforeRedraw() const {
        context->hideAllAdditionalObjects();
        context->context().DefaultDrawer()->ShadingAspect()->Aspect()->SetShaderProgram(depthMapShader);
        context->context().DefaultDrawer()->WireAspect()->Aspect()->SetShaderProgram(depthMapShader);
        context->context().DefaultDrawer()->FaceBoundaryAspect()->Aspect()->SetShaderProgram(depthMapShader);
        context->context().Display(&context->getAisDesk(), Standard_False);
    }

    void afterRedraw() const {
//        context->context().DefaultDrawer()->ShadingAspect()->Aspect()->SetShaderProgram(origShader);
//        context->context().DefaultDrawer()->WireAspect()->Aspect()->SetShaderProgram(origShader);
//        context->context().DefaultDrawer()->FaceBoundaryAspect()->Aspect()->SetShaderProgram(origShader);
        context->showAllAdditionalObjects();
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
        RedrawImmediate();
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

        const gp_Trsf trsf = context->getLsrHeadTransform();
        const gp_Quaternion rotation = trsf.GetRotation();
        gp_Vec orient(1, 0, 0);
        const gp_Vec orient_rot = rotation.Multiply(orient);
        SetUp(orient_rot.X(), orient_rot.Y(), orient_rot.Z());

        RedrawImmediate();
    }

    CInteractiveContext& getContext() const { return *context; }

private:
    CInteractiveContext * const context;
    Handle(Graphic3d_ShaderProgram) depthMapShader;
    Handle(Graphic3d_ShaderProgram) origShader;
};



class CDepthMapViewportPrivate : public AIS_ViewController
{
    friend class CDepthMapViewport;

    Handle(CDepthMapV3dView) view;
    Handle(CAspectWindow) aspect;
};



CDepthMapViewport::CDepthMapViewport(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CDepthMapViewportPrivate())
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    setBackgroundRole(QPalette::NoRole);
    setFocusPolicy(Qt::StrongFocus);
}

CDepthMapViewport::~CDepthMapViewport()
{
    delete d_ptr;
}

void CDepthMapViewport::setContext(CInteractiveContext &context)
{
    d_ptr->view = new CDepthMapV3dView(context, V3d_ORTHOGRAPHIC);

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

void CDepthMapViewport::updatePosition()
{
    d_ptr->view->updatePosition();
}

void CDepthMapViewport::createSnapshot(const char *fname, const size_t width, const size_t height)
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

void CDepthMapViewport::setScale(const double scale)
{
    if (scale > 0)
        d_ptr->view->setScale(scale);
}

QPaintEngine *CDepthMapViewport::paintEngine() const
{
    return nullptr;
}

void CDepthMapViewport::paintEvent(QPaintEvent *)
{
    d_ptr->view->InvalidateImmediate();
    d_ptr->FlushViewEvents(&d_ptr->view->getContext().context(), d_ptr->view, Standard_True);
}

void CDepthMapViewport::resizeEvent(QResizeEvent *)
{
    d_ptr->view->MustBeResized();
}
