#ifndef CBREPLOADER_H
#define CBREPLOADER_H

#include "cabstractmodelloader.h"

class CBrepLoader : public CAbstractModelLoader
{
public:
    CBrepLoader();

protected:
    TopoDS_Shape loadPrivate(const char *fName) final;
};

#endif // CBREPLOADER_H
