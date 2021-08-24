#ifndef GUI_TYPES_H
#define GUI_TYPES_H

#include <cmath>

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

enum EN_UiStates
{
    ENUS_CALIBRATION,
    ENUS_TASK_EDITING,
    ENUS_BOT_WORKED
};

enum EN_BotTaskTypes
{
    ENBTT_MOVE,
    ENBTT_DRILL,
    ENBTT_MARK
};
typedef int TBotTaskType;

typedef double TDistance;
typedef double TDegree;
typedef double TScale;

struct SVertex
{
    SVertex(const TDistance X = 0., const TDistance Y = 0, const TDistance Z = 0) :
        x(X), y(Y), z(Z) { }

    inline bool isEqual(const SVertex &other,
                        const TDistance precision = 0.) const {
        if (std::abs(x - other.x) < precision &&
                std::abs(y - other.y) < precision &&
                std::abs(z - other.z) < precision)
                return true;
        return false;
    }

    TDistance x, y, z;
};

struct SRotationAngle
{
    SRotationAngle(const TDegree alpha = 0., const TDegree beta = 0, const TDegree gamma = 0) :
        x(alpha), y(beta), z(gamma) { }

    inline bool isEqual(const SRotationAngle &other,
                        const TDegree precision = 0.) const {
        if (std::abs(x - other.x) < precision &&
                std::abs(y - other.y) < precision &&
                std::abs(z - other.z) < precision)
                return true;
        return false;
    }

    TDistance x, y, z;
};

struct SCalibPoint
{
    SCalibPoint() {}

    SVertex globalPos, botPos;
};

struct STaskPoint
{
    STaskPoint() :
        taskType(ENBTT_MOVE),
        normal(SVertex(0., 0., 1.)),
        zSimmetry(false),
        bNeedCalib(false) { }

    TBotTaskType taskType;
    SVertex globalPos;
    SRotationAngle angle;
    SVertex normal;
    bool zSimmetry;
    bool bNeedCalib;
};

struct SPathPoint
{
    SPathPoint() {}

    SVertex globalPos;
    SRotationAngle angle;
};

template <typename Key, typename Value, typename Map>
inline static Value extract_map_value(const Map &map,
                                      const Key key,
                                      const Value defaultVl = Value()) {
    Value res = defaultVl;
    const typename Map::const_iterator it = map.find(key);
    if (it != map.cend())
        res = it->second;
    return res;
}

enum EN_ModelPurpose
{
    ENMP_DESK,
    ENMP_PART,
    ENMP_LSRHEAD,
    ENMP_GRIP
};

}

#endif // GUI_TYPES_H
