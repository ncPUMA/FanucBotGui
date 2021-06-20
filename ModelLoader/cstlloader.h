#ifndef CSTLLOADER_H
#define CSTLLOADER_H

#include "cabstractmodelloader.h"

class CStlLoader : public CAbstractModelLoader
{
public:
    CStlLoader();

protected:
    TopoDS_Shape loadPrivate(const char *fName) final;
};

#endif // CSTLLOADER_H
