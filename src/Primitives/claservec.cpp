#include "claservec.h"

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_Text.hxx>

CLaserVec::CLaserVec(const gp_Pnt2d& thePnt1,
                     const gp_Pnt2d& thePnt2,
                     Standard_Real theArrowLength)
: myPnt (gp_Pnt (thePnt1.X(), thePnt1.Y(), 0.0)),
  myLength (0.0),
  myArrowLength (theArrowLength)
{
  gp_Vec aVec (thePnt2.X() - thePnt1.X(), thePnt2.Y() - thePnt1.Y(), 0.0);
  myDir = gp_Dir(aVec);
  myLength = aVec.Magnitude();
}

void CLaserVec::Compute (const Handle(PrsMgr_PresentationManager3d)& ,
                         const Handle(Prs3d_Presentation)& thePrs,
                         const Standard_Integer theMode)
{
    if (theMode != 0)
    {
        return;
    }

    // Set style for arrow
    Handle(Prs3d_ArrowAspect) anArrowAspect = myDrawer->ArrowAspect();
    anArrowAspect->SetLength(myArrowLength);

    gp_Pnt aLastPoint = myPnt;
    aLastPoint.Translate(myLength*gp_Vec(myDir));

    // Draw Line
    {
        myDrawer->SetLineAspect(new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_SOLID, 2.));
        Handle(Graphic3d_Group) aLineGroup = thePrs->NewGroup();
        aLineGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
        Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
        aPrims->AddVertex(myPnt);
        aPrims->AddVertex(aLastPoint);
        aLineGroup->AddPrimitiveArray(aPrims);

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
