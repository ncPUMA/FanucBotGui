#include "cadvanceddepthmapviewport.h"

#include <QFile>
#include <QDebug>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_LineAspect.hxx>

class CAdvancedDepthMapViewportPrivate
{
    friend class CAdvancedDepthMapViewport;

    CAdvancedDepthMapViewportPrivate()
        : depthMapShader(new Graphic3d_ShaderProgram()) {
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

        if (!depthMapShader->AttachShader(vertex) || !depthMapShader->AttachShader(fragment))
            qDebug() << "ERROR: CAdvancedDepthMapViewportPrivate: cannot attach shader";
    }

    bool modelShapeChanged(AIS_InteractiveContext &context,
                           const GUI_TYPES::EN_ShapeType model,
                           const TopoDS_Shape &shape) {
        bool needUpdate = false;
        switch(model)
        {
            using namespace GUI_TYPES;

            case ENST_DESK:
                context.Remove(ais_desk, Standard_False);
                ais_desk = new AIS_Shape(shape);
                context.SetDisplayMode(ais_desk, AIS_Shaded, Standard_False);
                context.Display(ais_desk, Standard_False);
                context.Deactivate(ais_desk);
//                ais_desk->Attributes()->ShadingAspect()->Aspect()->SetShaderProgram(depthMapShader);
                needUpdate = true;
                break;
            case ENST_PART:
                context.Remove(ais_part, Standard_False);
                ais_part = new AIS_Shape(shape);
                context.SetDisplayMode(ais_part, AIS_Shaded, Standard_False);
                context.Display(ais_part, Standard_False);
                context.Deactivate(ais_part);
//                ais_part->Attributes()->ShadingAspect()->Aspect()->SetShaderProgram(depthMapShader);
                needUpdate = true;
                break;
            case ENST_LSRHEAD:
            case ENST_GRIP:
                break;
        }
        return needUpdate;
    }

    bool modelTransformChanged(AIS_InteractiveContext &context,
                               const GUI_TYPES::EN_ShapeType model,
                               const gp_Trsf &trsf) {
        bool needUpdate = false;
        switch(model)
        {
            using namespace GUI_TYPES;

            case ENST_DESK:
                context.SetLocation(ais_desk, trsf);
                needUpdate = true;
                break;
            case ENST_PART:
                context.SetLocation(ais_part, trsf);
                needUpdate = true;
                break;
            case ENST_LSRHEAD:
            case ENST_GRIP:
                break;
        }
        return needUpdate;
    }

    Handle(AIS_Shape) ais_desk;
    Handle(AIS_Shape) ais_part;
    Handle(Graphic3d_ShaderProgram) depthMapShader;
};



CAdvancedDepthMapViewport::CAdvancedDepthMapViewport(QWidget *parent)
    : CAdvancedViewport(parent)
    , d_ptr(new CAdvancedDepthMapViewportPrivate())
{

}

CAdvancedDepthMapViewport::~CAdvancedDepthMapViewport()
{
    delete d_ptr;
}

void CAdvancedDepthMapViewport::initPrivate(AIS_InteractiveContext &context)
{
    context.DefaultDrawer()->ShadingAspect()->Aspect()->SetShaderProgram(d_ptr->depthMapShader);
    context.DefaultDrawer()->WireAspect()->Aspect()->SetShaderProgram(d_ptr->depthMapShader);
    context.DefaultDrawer()->FaceBoundaryAspect()->Aspect()->SetShaderProgram(d_ptr->depthMapShader);
}

bool CAdvancedDepthMapViewport::modelShapeChangedPrivate(AIS_InteractiveContext &context,
                                                         const GUI_TYPES::EN_ShapeType model,
                                                         const TopoDS_Shape &shape)
{
    return d_ptr->modelShapeChanged(context, model, shape);
}

bool CAdvancedDepthMapViewport::modelTransformChangedPrivate(AIS_InteractiveContext &context,
                                                             const GUI_TYPES::EN_ShapeType model,
                                                             const gp_Trsf &trsf)
{
    return d_ptr->modelTransformChanged(context, model, trsf);
}
