#include "cigesloader.h"

#include <IGESControl_Reader.hxx>

CIgesLoader::CIgesLoader() :
    CAbstractModelLoader()
{

}

TopoDS_Shape CIgesLoader::loadPrivate(const char *fName)
{
    TopoDS_Shape result;
    IGESControl_Reader aReader;
    int status = aReader.ReadFile (fName);
    if (status == IFSelect_RetDone)
    {
        aReader.TransferRoots();
        result = aReader.OneShape();
    }
    return result;
}
