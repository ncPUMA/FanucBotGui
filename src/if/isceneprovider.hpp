#ifndef ISCENEPROVIDER_HPP
#define ISCENEPROVIDER_HPP

#include "TopoDS_Shape.hxx"

/**
 * @brief The ISceneProvider interface
 *
 * Implementations of this interface
 * should provide access to some topologies
 */
class ISceneProvider
{
public:
    enum ShapeType{
        SHAPE_PART,
        SHAPE_GRIP,
        SHAPE_LASERHEAD,
        SHAPE_FIXTURE
    };
    virtual ~ISceneProvider() = default;

    // get shape
    virtual const TopoDS_Shape &getShape(ShapeType type) = 0;
};

/**
 * @brief The ISceneObserver interface
 *
 * Implementations of this interface
 * should be provided with ISceneProvider
 */
class ISceneObserver
{
public:
    virtual ~ISceneObserver() = default;

    virtual void setSceneProvider(ISceneProvider *scene_provider) = 0;
};

#endif // ISCENEPROVIDER_HPP
