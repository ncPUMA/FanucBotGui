#ifndef SGUISETTINGS_H
#define SGUISETTINGS_H

#include "gui_types.h"

struct SGuiSettings
{
    SGuiSettings() :
        msaa(0),
        partTrX(0),
        partTrY(0),
        partTrZ(0),
        partCenterX(0),
        partCenterY(0),
        partCenterZ(0),
        partRotationX(0),
        partRotationY(0),
        partRotationZ(0),
        partScale(0)
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
};

#endif // SGUISETTINGS_H
