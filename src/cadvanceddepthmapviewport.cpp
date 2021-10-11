#include "cadvanceddepthmapviewport.h"

#include <QFile>
#include <QDebug>

#include <AIS_InteractiveContext.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_LineAspect.hxx>

class CAdvancedDepthMapViewportPrivate
{
    friend class CAdvancedDepthMapViewport;

    void init(AIS_InteractiveContext &context) {
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

        Handle(Graphic3d_ShaderProgram) depthMapShader = new Graphic3d_ShaderProgram();
        if (!depthMapShader->AttachShader(vertex) || !depthMapShader->AttachShader(fragment))
            qDebug() << "ERROR: CAdvancedDepthMapViewportPrivate: cannot attach shader";
        context.DefaultDrawer()->ShadingAspect()->Aspect()->SetShaderProgram(depthMapShader);
        context.DefaultDrawer()->WireAspect()->Aspect()->SetShaderProgram(depthMapShader);
        context.DefaultDrawer()->FaceBoundaryAspect()->Aspect()->SetShaderProgram(depthMapShader);
    }
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
    d_ptr->init(context);
}
