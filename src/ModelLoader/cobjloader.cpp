#include "cobjloader.h"

#include <RWObj.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Poly_Triangulation.hxx>

CObjLoader::CObjLoader() :
    CAbstractModelLoader()
{

}

TopoDS_Shape CObjLoader::loadPrivate(const char *fName)
{
    TopoDS_Shape result;
    Handle(Poly_Triangulation) mesh = RWObj::ReadFile(fName);
    if (!mesh.IsNull())
    {
        BRep_Builder shellBuilder;
        TopoDS_Shell shell;
        shellBuilder.MakeShell(shell);
        for (const Poly_Triangle triangle : mesh->Triangles())
        {
            Standard_Integer index0, index1, index2;
            triangle.Get(index0, index1, index2);
            const gp_Pnt pnt0 = mesh->Node(index0);
            const gp_Pnt pnt1 = mesh->Node(index1);
            const gp_Pnt pnt2 = mesh->Node(index2);
            const TopoDS_Wire wire = BRepBuilderAPI_MakePolygon(pnt0, pnt1, pnt2, Standard_True);
            const TopoDS_Face face = BRepBuilderAPI_MakeFace(wire, Standard_True);
            shellBuilder.Add(shell, face);
        }
        result = shell;
        //Handle(AIS_Triangulation) aisMesh = new AIS_Triangulation(mesh);
    }
    return result;
}
