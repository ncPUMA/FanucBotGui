#ifndef POSITION_H
#define POSITION_H

#include <gp_Vec.hxx>
#include <gp_Quaternion.hxx>

struct position_t
{
    gp_Vec t;           // translation
    gp_Quaternion r;    // rotation
};

#endif // POSITION_H
