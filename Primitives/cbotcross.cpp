#include "cbotcross.h"

#include <sstream>

#include <AIS_Point.hxx>
#include <Geom_CartesianPoint.hxx>
#include <AIS_TextLabel.hxx>

class CBotCrossPrivate
{
    friend class CBotCross;

    CBotCrossPrivate() {
        gp_Pnt pnt(0., 0., 0.);
        Handle(Geom_CartesianPoint) geomPoint = new Geom_CartesianPoint(pnt);
        point = new AIS_Point(geomPoint);
        point->SetColor(Quantity_Color(.15, .15, .15, Quantity_TOC_RGB));

        txtCoord = new AIS_TextLabel();
        txtCoord->SetColor(Quantity_Color(.15, .15, .15, Quantity_TOC_RGB));
        txtCoord->SetText("Coord: 0. 0. 0.\n   Ang: 0. 0. 0.");
        txtCoord->SetPosition(gp_Pnt(pnt.X(), pnt.Y(), pnt.Z()));
    }

    Handle(AIS_Point) point;
    Handle(AIS_TextLabel) txtCoord;
};



CBotCross::CBotCross() :
    d_ptr(new CBotCrossPrivate())
{

}

CBotCross::~CBotCross()
{
    delete d_ptr;
}

NCollection_Vector<Handle (AIS_InteractiveObject)> CBotCross::objects(const char *text) const
{
    NCollection_Vector<Handle (AIS_InteractiveObject)> result;
    result.Append(d_ptr->point);
    d_ptr->txtCoord->SetText(TCollection_ExtendedString(text, Standard_True));
    result.Append(d_ptr->txtCoord);
    return result;
}
