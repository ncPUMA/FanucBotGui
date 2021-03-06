#include "cadvancedsnapshotviewport.h"

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <V3d_DirectionalLight.hxx>
#include <AIS_Shape.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_LineAspect.hxx>

static const Quantity_Color BG_CLR   = Quantity_Color(0., 0., 0., Quantity_TOC_RGB);
static const Quantity_Color FACE_CLR = Quantity_Color(1., 1., 1., Quantity_TOC_RGB);

class CAdvancedSnapshotViewportPrivate
{
    friend class CAdvancedSnapshotViewport;

    CAdvancedSnapshotViewportPrivate()
        : drawer(new Prs3d_Drawer())
        , light(new V3d_DirectionalLight(gp_Dir(0, 0, -1), Quantity_NOC_WHITE, Standard_True)) {
        Handle(Prs3d_ShadingAspect) aShAspect = drawer->ShadingAspect();
        aShAspect->SetColor(FACE_CLR);
        drawer->SetShadingAspect(aShAspect);
        drawer->SetShadingModel(Graphic3d_TOSM_FACET);

        Handle(Prs3d_LineAspect) lAspect = drawer->FaceBoundaryAspect();
        lAspect->SetColor(FACE_CLR);
        drawer->SetFaceBoundaryAspect(lAspect);
        drawer->SetFaceBoundaryDraw(Standard_False);
    }

    Handle(Prs3d_Drawer) drawer;
    Handle(V3d_DirectionalLight) light;
};



CAdvancedSnapshotViewport::CAdvancedSnapshotViewport(QWidget *parent)
    : CAdvancedViewport(parent)
    , d_ptr(new CAdvancedSnapshotViewportPrivate())
{

}

CAdvancedSnapshotViewport::~CAdvancedSnapshotViewport()
{
    delete d_ptr;
}

void CAdvancedSnapshotViewport::initPrivate(AIS_InteractiveContext &context, V3d_View &view)
{
    (void)context;

    view.SetLightOn(d_ptr->light);
    view.SetBackgroundColor(BG_CLR);
}

bool CAdvancedSnapshotViewport::modelShapeChangedPrivate(AIS_InteractiveContext &context,
                                                         const GUI_TYPES::EN_ShapeType model,
                                                         AIS_Shape &shape)
{
    (void)model;

    context.SetLocalAttributes(&shape, d_ptr->drawer, Standard_False);
    context.Redisplay(&shape, Standard_False);
    return true;
}
