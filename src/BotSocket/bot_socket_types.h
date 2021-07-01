#ifndef BOT_SOCKET_TYPES_H
#define BOT_SOCKET_TYPES_H

#include "../gui_types.h"

namespace BotSocket
{

enum EN_BotState
{
    ENBS_FALL,
    ENBS_NOT_ATTACHED,
    ENBS_ATTACHED
};

typedef GUI_TYPES::TDistance      TDistance;
typedef GUI_TYPES::TDegree        TDegree;
typedef GUI_TYPES::SVertex        SPosition;
typedef GUI_TYPES::SRotationAngle SRotationAngle;

struct SBotPosition
{
    SBotPosition() { }
    SBotPosition(const TDistance X,
                 const TDistance Y,
                 const TDistance Z,
                 const TDegree alpha,
                 const TDegree beta,
                 const TDegree gamma) :
        globalPos(X, Y, Z),
        globalRotation(alpha, beta, gamma) { }

    inline bool isEqual(const SBotPosition &other,
                        const TDistance distPrecision = 0., const TDegree rotPrecition = 0.) const {
        if (globalPos.isEqual(other.globalPos, distPrecision) &&
                globalRotation.isEqual(globalRotation, rotPrecition))
            return true;
        return false;
    }

    SPosition        globalPos;
    SRotationAngle   globalRotation;
};

enum EN_ShapeType
{
    ENST_DESK,
    ENST_PART,
    ENST_LSRHEAD,
    ENST_GRIP
};

}

#endif // BOT_SOCKET_TYPES_H
