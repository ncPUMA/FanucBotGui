#ifndef CJSONFILEPOINTSSAVER_H
#define CJSONFILEPOINTSSAVER_H

#include "cabstractpointssaver.h"

class CJsonFilePointsSaverPrivate;

class CJsonFilePointsSaver : public CAbstractPointsSaver
{
public:
    CJsonFilePointsSaver();
    ~CJsonFilePointsSaver();

    void setFileName(const char *fName);

protected:
    bool savePointsPrivate(const std::vector <GUI_TYPES::STaskPoint> &taskPoints,
                           const std::vector <GUI_TYPES::SHomePoint> &homePoints) final;
    bool loadPointsPrivate(std::vector <GUI_TYPES::STaskPoint> &taskPoints,
                           std::vector <GUI_TYPES::SHomePoint> &homePoints) final;

private:
    CJsonFilePointsSaverPrivate * const d_ptr;
};

#endif // CJSONFILEPOINTSSAVER_H
