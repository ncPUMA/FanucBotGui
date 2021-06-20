#ifndef CIGESLOADER_H
#define CIGESLOADER_H

#include "cabstractmodelloader.h"

class CIgesLoader : public CAbstractModelLoader
{
public:
    CIgesLoader();

protected:
    TopoDS_Shape loadPrivate(const char *fName) final;
};

#endif // CIGESLOADER_H
