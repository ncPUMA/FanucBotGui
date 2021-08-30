#ifndef CPATHVEC_H
#define CPATHVEC_H

#include <AIS_InteractiveObject.hxx>

//! AIS interactive Object for vector with arrow and text
class CPathVec : public AIS_InteractiveObject
{
    DEFINE_STANDARD_RTTI_INLINE(CPathVec, AIS_InteractiveObject)
    public:

        CPathVec()
      : myLength (1.0),
        myArrowLength (1.0)
    {}

    CPathVec (const gp_Pnt& thePnt,
              const gp_Dir& theDir,
              Standard_Real theLength = 1,
              Standard_Real theArrowLength = 1)
        : myPnt (thePnt),
          myDir (theDir),
          myLength (theLength),
          myArrowLength (theArrowLength)
    {
        //
    }

    CPathVec (const gp_Pnt& thePnt,
              const gp_Vec& theVec,
              Standard_Real theArrowLength = 1)
        : myPnt (thePnt),
          myDir (theVec),
          myLength (theVec.Magnitude()),
          myArrowLength (theArrowLength)
    {
        //
    }

    CPathVec (const gp_Pnt& thePnt1,
              const gp_Pnt& thePnt2,
              Standard_Real theArrowLength = 5)
        : myPnt (thePnt1),
          myDir (gp_Vec(thePnt1, thePnt2)),
          myLength (gp_Vec(thePnt1, thePnt2).Magnitude()),
          myArrowLength (theArrowLength)
    {
        //
    }

    CPathVec (const gp_Pnt2d& thePnt2d,
              const gp_Dir2d& theDir2d,
              Standard_Real theLength = 1,
              Standard_Real theArrowLength = 1)
        : myPnt (gp_Pnt(thePnt2d.X(), thePnt2d.Y(), 0.0)),
          myDir (gp_Dir(theDir2d.X(), theDir2d.Y(), 0.0)),
          myLength (theLength),
          myArrowLength (theArrowLength)
    {
        //
    }

    CPathVec (const gp_Pnt2d& thePnt2d,
              const gp_Vec2d& theVec2d,
              Standard_Real theArrowLength = 1)
        : myPnt (gp_Pnt(thePnt2d.X(), thePnt2d.Y(), 0.0)),
          myDir (gp_Dir(theVec2d.X(), theVec2d.Y(), 0.0)),
          myLength (theVec2d.Magnitude()),
          myArrowLength (theArrowLength)
    {
        //
    }

    CPathVec (const gp_Pnt2d& thePnt1,
              const gp_Pnt2d& thePnt2,
              Standard_Real theArrowLength = 1);

    void SetText (const TCollection_AsciiString& theText)
    {
        myText = theText;
    }

    void SetLineAspect (const Handle(Prs3d_LineAspect)& theAspect)
    {
        myDrawer->SetLineAspect(theAspect);
    }

private:

    //! Return TRUE for supported display modes (only mode 0 is supported).
    virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0; }

    //! Compute presentation.
    virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                          const Handle(Prs3d_Presentation)& thePrs,
                          const Standard_Integer theMode) Standard_OVERRIDE;

    //! Compute selection (not implemented).
    virtual void ComputeSelection (const Handle(SelectMgr_Selection)&,
                                   const Standard_Integer) Standard_OVERRIDE {}

private:
    gp_Pnt myPnt;
    gp_Dir myDir;
    Standard_Real myLength;
    Standard_Real myArrowLength;
    TCollection_AsciiString myText;
};

#endif // CPATHVEC_H
