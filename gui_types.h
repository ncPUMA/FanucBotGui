#ifndef GUI_TYPES_H
#define GUI_TYPES_H

namespace GUI_TYPES
{

enum EN_MSAA
{
    ENMSAA_OFF = 0,
    ENMSAA_2   = 2,
    ENMSAA_4   = 4,
    ENMSAA_8   = 8
};
typedef int TMSAA;

enum EN_CoordSystems
{
    ENCS_RIGHT,
    ENCS_LEFT
};
typedef int TCoordSystem;

}

#endif // GUI_TYPES_H
