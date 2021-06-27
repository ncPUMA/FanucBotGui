#ifndef SGUISETTINGS_H
#define SGUISETTINGS_H

#include "gui_types.h"

struct SGuiSettings
{
    SGuiSettings() :
        //common
        msaa(0),
        //The Part
        partTrX(0),
        partTrY(0),
        partTrZ(0),
        partCenterX(0),
        partCenterY(0),
        partCenterZ(0),
        partRotationX(0),
        partRotationY(0),
        partRotationZ(0),
        partScale(0),
        //The Part
        deskTrX(0),
        deskTrY(0),
        deskTrZ(0),
        deskCenterX(0),
        deskCenterY(0),
        deskCenterZ(0),
        deskRotationX(0),
        deskRotationY(0),
        deskRotationZ(0),
        deskScale(0),
        //The Grip
        gripTrX(0),
        gripTrY(0),
        gripTrZ(0),
        gripCenterX(0),
        gripCenterY(0),
        gripCenterZ(0),
        gripRotationX(0),
        gripRotationY(0),
        gripRotationZ(0),
        gripScale(0)
    { }

    //common
    GUI_TYPES::TMSAA msaa;

    //The Part
    double partTrX;
    double partTrY;
    double partTrZ;
    double partCenterX;
    double partCenterY;
    double partCenterZ;
    double partRotationX;
    double partRotationY;
    double partRotationZ;
    double partScale;

    //The Desk
    double deskTrX;
    double deskTrY;
    double deskTrZ;
    double deskCenterX;
    double deskCenterY;
    double deskCenterZ;
    double deskRotationX;
    double deskRotationY;
    double deskRotationZ;
    double deskScale;

    //The Grip
    double gripTrX;
    double gripTrY;
    double gripTrZ;
    double gripCenterX;
    double gripCenterY;
    double gripCenterZ;
    double gripRotationX;
    double gripRotationY;
    double gripRotationZ;
    double gripScale;
};

#endif // SGUISETTINGS_H
