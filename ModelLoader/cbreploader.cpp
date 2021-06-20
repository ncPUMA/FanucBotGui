#include "cbreploader.h"

#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

CBrepLoader::CBrepLoader() :
    CAbstractModelLoader()
{

}

TopoDS_Shape CBrepLoader::loadPrivate(const char *fName)
{
    TopoDS_Shape result;
    BRep_Builder aBuilder;
    BRepTools::Read(result, fName , aBuilder);
    return result;
}
