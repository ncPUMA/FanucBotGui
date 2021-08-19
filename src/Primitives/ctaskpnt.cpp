#include "ctaskpnt.h"

#include <cmath>

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_Text.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <IntCurvesFace_ShapeIntersector.hxx>

CTaskPnt::CTaskPnt(const gp_Pnt2d& thePnt1,
                   const gp_Pnt2d& thePnt2)
: myPnt (gp_Pnt (thePnt1.X(), thePnt1.Y(), 0.0)),
  myLength (0.0)
{
  gp_Vec aVec (thePnt2.X() - thePnt1.X(), thePnt2.Y() - thePnt1.Y(), 0.0);
  myDir = gp_Dir(aVec);
  myLength = aVec.Magnitude();
}

void CTaskPnt::Compute (const Handle(PrsMgr_PresentationManager3d)& ,
                        const Handle(Prs3d_Presentation)& thePrs,
                        const Standard_Integer theMode)
{
    if (theMode != 0)
    {
        return;
    }

    // Set style for arrow
    Handle(Prs3d_ArrowAspect) anArrowAspect = myDrawer->ArrowAspect();
    anArrowAspect->SetLength(0);

    gp_Pnt aLastPoint = myPnt;
    aLastPoint.Translate(myLength * gp_Vec(myDir));

    // Draw Line
//    {
//        Quantity_Color clr(Quantity_NOC_STEELBLUE3);
//        if (HasColor())
//            Color(clr);
//        myDrawer->SetLineAspect(new Prs3d_LineAspect(clr, Aspect_TOL_SOLID, 1.5));
//        Handle(Graphic3d_Group) aLineGroup = thePrs->NewGroup();
//        aLineGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
//        Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
//        aPrims->AddVertex(myPnt);
//        aPrims->AddVertex(aLastPoint);
//        aLineGroup->AddPrimitiveArray(aPrims);
//        Handle(Graphic3d_ArrayOfSegments) aPrims2 = new Graphic3d_ArrayOfSegments(2);
//        aPrims2->AddVertex(myPnt);
//        gp_Ax3 axZ(myPnt, myDir);
//        gp_Pnt xPnt = myPnt;
//        xPnt.Translate(myLength / 3. * axZ.XDirection());
//        aPrims2->AddVertex(xPnt);
//        aLineGroup->AddPrimitiveArray(aPrims2);

//        // Draw arrow
//        Prs3d_Arrow::Draw(aLineGroup, aLastPoint, myDir, anArrowAspect->Angle(), anArrowAspect->Length());
//    }

    {
        Quantity_Color clr(Quantity_NOC_STEELBLUE3);
        if (HasColor())
            Color(clr);
        myDrawer->SetLineAspect(new Prs3d_LineAspect(clr, Aspect_TOL_SOLID, 1.5));
        Handle(Graphic3d_Group) aLineGroup = thePrs->NewGroup();
        aLineGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
        Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
        const gp_Pnt start(0., 0., 0.);
        const gp_Pnt zPnt(0., 0., myLength);
        const gp_Pnt xPnt(myLength / 3., 0., 0.);
        aPrims->AddVertex(start);
        aPrims->AddVertex(zPnt);
        aLineGroup->AddPrimitiveArray(aPrims);
        Handle(Graphic3d_ArrayOfSegments) aPrims2 = new Graphic3d_ArrayOfSegments(2);
        aPrims2->AddVertex(start);
        aPrims2->AddVertex(xPnt);
        aLineGroup->AddPrimitiveArray(aPrims2);

        // Draw arrow
        Prs3d_Arrow::Draw(aLineGroup, aLastPoint, myDir, anArrowAspect->Angle(), anArrowAspect->Length());
    }

    // Draw text
    if (!myText.IsEmpty())
    {
        gp_Pnt aTextPosition = aLastPoint;
        Prs3d_Text::Draw (thePrs->NewGroup(), myDrawer->TextAspect(), myText, aTextPosition);
    }
}
