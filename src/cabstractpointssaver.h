#ifndef CABSTRACTPOINTSSAVER_H
#define CABSTRACTPOINTSSAVER_H

#include <vector>

#include "gui_types.h"

class CAbstractPointsSaver
{
public:
    virtual ~CAbstractPointsSaver() { }

    bool savePoints(const std::vector <GUI_TYPES::STaskPoint> &taskPoints,
                    const std::vector <GUI_TYPES::SHomePoint> &homePoints) {
        return savePointsPrivate(taskPoints, homePoints);
    }
    bool loadPoints(std::vector <GUI_TYPES::STaskPoint> &taskPoints,
                    std::vector <GUI_TYPES::SHomePoint> &homePoints) {
        return loadPointsPrivate(taskPoints, homePoints);
    }

protected:
    CAbstractPointsSaver() { }

    virtual bool savePointsPrivate(const std::vector <GUI_TYPES::STaskPoint> &taskPoints,
                                   const std::vector <GUI_TYPES::SHomePoint> &homePoints) = 0;
    virtual bool loadPointsPrivate(std::vector <GUI_TYPES::STaskPoint> &taskPoints,
                                   std::vector <GUI_TYPES::SHomePoint> &homePoints) = 0;
};

#endif // CABSTRACTPOINTSSAVER_H
