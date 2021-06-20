#include "cmodelloaderfactorymethod.h"

#include <vector>

#include <QStringList>

#include "cbreploader.h"
#include "csteploader.h"
#include "cigesloader.h"
#include "cstlloader.h"
#include "cobjloader.h"

class CEmptyModelLoader : public CAbstractModelLoader
{
public:
    CEmptyModelLoader() : CAbstractModelLoader() { }

protected:
    TopoDS_Shape loadPrivate(const char *) final {
        return TopoDS_Shape();
    }
};



class CModelLoaderFactoryMethodPrivate
{
    friend class CModelLoaderFactoryMethod;

    CModelLoaderFactoryMethodPrivate() {
        appendLoader("STEP (*.step *.stp)", new CStepLoader());
        appendLoader("BREP (*.brep)", new CBrepLoader());
        appendLoader("IGES (*.iges)", new CIgesLoader());
        appendLoader("STL (*.stl)"  , new CStlLoader());
        appendLoader("OBJ (*.obj)"  , new CObjLoader());
    }

    ~CModelLoaderFactoryMethodPrivate() {
        for(auto pair : loaders)
            delete pair.second;
    }

    CAbstractModelLoader& loader(const QString &filter) {
        CAbstractModelLoader * result = &emptyLoader;
        for(auto pair : loaders) {
            if (pair.first == filter) {
                result = pair.second;
                break;
            }
        }
        return *result;
    }

    void appendLoader(const char *format, CAbstractModelLoader * const loader) {
        loaders.push_back(std::make_pair(QString(format), loader));
    }

    std::vector <std::pair <QString, CAbstractModelLoader *> > loaders;
    CEmptyModelLoader emptyLoader;
};



CModelLoaderFactoryMethod::CModelLoaderFactoryMethod() :
    d_ptr(new CModelLoaderFactoryMethodPrivate())
{

}

CModelLoaderFactoryMethod::~CModelLoaderFactoryMethod()
{
    delete d_ptr;
}

QString CModelLoaderFactoryMethod::supportedFilters() const
{
    QStringList ls;
    for(auto pair : d_ptr->loaders)
        ls << pair.first;
    return ls.join(";;");
}

CAbstractModelLoader &CModelLoaderFactoryMethod::loader(const QString &filter)
{
    return d_ptr->loader(filter);
}
