#ifndef SGUISETTINGS_H
#define SGUISETTINGS_H

#include "gui_types.h"

namespace GUI_TYPES
{

struct SGuiSettings
{
    SGuiSettings() :
        //common
        msaa(0),
        snapshotScale(5.),
        //The Part
        partTrX(0.),
        partTrY(0.),
        partTrZ(0.),
        partCenterX(0.),
        partCenterY(0.),
        partCenterZ(0.),
        partRotationX(0.),
        partRotationY(0.),
        partRotationZ(0.),
        partScale(1.),
        //The Desk
        deskTrX(0.),
        deskTrY(0.),
        deskTrZ(0.),
        deskCenterX(0.),
        deskCenterY(0.),
        deskCenterZ(0.),
        deskRotationX(0.),
        deskRotationY(0.),
        deskRotationZ(0.),
        deskScale(1.),
        //The Laser Head
        lheadTrX(0.),
        lheadTrY(0.),
        lheadTrZ(0.),
        lheadCenterX(0.),
        lheadCenterY(0.),
        lheadCenterZ(0.),
        lheadRotationX(0.),
        lheadRotationY(0.),
        lheadRotationZ(0.),
        lheadScale(1.),
        lheadLsrTrX(0.),
        lheadLsrTrY(0.),
        lheadLsrTrZ(0.),
        lheadLsrNormalX(0.),
        lheadLsrNormalY(0.),
        lheadLsrNormalZ(0.),
        lheadLsrLenght(200.),
        lheadLsrClip(true),
        //The Grip
        gripTrX(0.),
        gripTrY(0.),
        gripTrZ(0.),
        gripCenterX(0.),
        gripCenterY(0.),
        gripCenterZ(0.),
        gripRotationX(0.),
        gripRotationY(0.),
        gripRotationZ(0.),
        gripScale(1.),
        gripVis(true)
    { }

    //common
    TMSAA msaa;
    TScale snapshotScale;

    //The Part
    TDistance partTrX;
    TDistance partTrY;
    TDistance partTrZ;
    TDistance partCenterX;
    TDistance partCenterY;
    TDistance partCenterZ;
    TDegree partRotationX;
    TDegree partRotationY;
    TDegree partRotationZ;
    TScale partScale;

    //The Desk
    TDistance deskTrX;
    TDistance deskTrY;
    TDistance deskTrZ;
    TDistance deskCenterX;
    TDistance deskCenterY;
    TDistance deskCenterZ;
    TDegree deskRotationX;
    TDegree deskRotationY;
    TDegree deskRotationZ;
    TScale deskScale;

    //The Laser Head
    TDistance lheadTrX;
    TDistance lheadTrY;
    TDistance lheadTrZ;
    TDistance lheadCenterX;
    TDistance lheadCenterY;
    TDistance lheadCenterZ;
    TDegree lheadRotationX;
    TDegree lheadRotationY;
    TDegree lheadRotationZ;
    TScale lheadScale;
    TDistance lheadLsrTrX;
    TDistance lheadLsrTrY;
    TDistance lheadLsrTrZ;
    TDistance lheadLsrNormalX;
    TDistance lheadLsrNormalY;
    TDistance lheadLsrNormalZ;
    TDistance lheadLsrLenght;
    bool lheadLsrClip;

    //The Grip
    TDistance gripTrX;
    TDistance gripTrY;
    TDistance gripTrZ;
    TDistance gripCenterX;
    TDistance gripCenterY;
    TDistance gripCenterZ;
    TDegree gripRotationX;
    TDegree gripRotationY;
    TDegree gripRotationZ;
    TScale gripScale;
    bool gripVis;
};

}

#endif // SGUISETTINGS_H
