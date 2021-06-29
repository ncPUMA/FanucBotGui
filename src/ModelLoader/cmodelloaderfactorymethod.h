#ifndef CMODELLOADERFACTORYMETHOD_H
#define CMODELLOADERFACTORYMETHOD_H

#include <QString>

class CModelLoaderFactoryMethodPrivate;
class CAbstractModelLoader;

class CModelLoaderFactoryMethod
{
public:
    CModelLoaderFactoryMethod();
    ~CModelLoaderFactoryMethod();

    QString supportedFilters() const;
    CAbstractModelLoader& loader(const QString &filter);

private:
    CModelLoaderFactoryMethodPrivate * const d_ptr;
};

#endif // CMODELLOADERFACTORYMETHOD_H
