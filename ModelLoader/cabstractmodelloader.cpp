#include "cabstractmodelloader.h"

CAbstractModelLoader::CAbstractModelLoader()
{

}

CAbstractModelLoader::~CAbstractModelLoader()
{

}

TopoDS_Shape CAbstractModelLoader::load(const char *fName)
{
    return loadPrivate(fName);
}
