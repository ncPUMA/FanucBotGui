#ifndef CABSTRACTMODELLOADER_H
#define CABSTRACTMODELLOADER_H

#include <TopoDS_Shape.hxx>

class CAbstractModelLoader
{
public:
    CAbstractModelLoader();
    virtual ~CAbstractModelLoader();

    TopoDS_Shape load(const char *fName);

protected:
    virtual TopoDS_Shape loadPrivate(const char *fName) = 0;

private:
    CAbstractModelLoader(const CAbstractModelLoader &) = delete;
    CAbstractModelLoader& operator=(const CAbstractModelLoader &) = delete;
};

#endif // CABSTRACTMODELLOADER_H
