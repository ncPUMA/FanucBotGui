#ifndef CTASKPNT_H
#define CTASKPNT_H

#include <AIS_InteractiveObject.hxx>

#include <NCollection_Vector.hxx>

class AIS_Shape;

//! AIS interactive Object for vector with arrow and text
class CTaskPnt : public AIS_InteractiveObject
{
    DEFINE_STANDARD_RTTI_INLINE(CTaskPnt, AIS_InteractiveObject)
    public:

        CTaskPnt()
      : myLength (1.0)
    {}

    CTaskPnt(const gp_Pnt& thePnt,
             const gp_Dir& theDir,
             Standard_Real theLength = 1)
        : myPnt (thePnt),
          myDir (theDir),
          myLength (theLength)
    {
        //
    }

    CTaskPnt(const gp_Pnt& thePnt,
             const gp_Vec& theVec)
        : myPnt (thePnt),
          myDir (theVec),
          myLength (theVec.Magnitude())
    {
        //
    }

    CTaskPnt(const gp_Pnt2d& thePnt2d,
             const gp_Dir2d& theDir2d,
             Standard_Real theLength = 1)
        : myPnt (gp_Pnt(thePnt2d.X(), thePnt2d.Y(), 0.0)),
          myDir (gp_Dir(theDir2d.X(), theDir2d.Y(), 0.0)),
          myLength (theLength)
    {
        //
    }

    CTaskPnt(const gp_Pnt2d& thePnt2d,
             const gp_Vec2d& theVec2d)
        : myPnt (gp_Pnt(thePnt2d.X(), thePnt2d.Y(), 0.0)),
          myDir (gp_Dir(theVec2d.X(), theVec2d.Y(), 0.0)),
          myLength (theVec2d.Magnitude())
    {
        //
    }

    CTaskPnt(const gp_Pnt2d& thePnt1,
             const gp_Pnt2d& thePnt2);

    void SetText (const TCollection_AsciiString& theText)
    {
        myText = theText;
    }

    void SetLineAspect (const Handle(Prs3d_LineAspect)& theAspect)
    {
        myDrawer->SetLineAspect(theAspect);
    }

    void setLenght(const double lenght)
    {
        myLength = lenght;
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
  TCollection_AsciiString myText;
};

#endif // CTASKPNT_H
