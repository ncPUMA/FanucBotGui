#ifndef IPARTREFERENCER_HPP
#define IPARTREFERENCER_HPP

#include <vector>
#include <QFuture>

#include "position.hpp"
#include "isceneprovider.hpp"

/**
 * @brief The IPartReferencer interface
 *
 * References part with some method
 *
 * Provides interface to transform positions from part coordinate system to robot
 */
class IPartReferencer
{
public:
    virtual ~IPartReferencer() = default;

    /**
     * @brief referencePart
     * asynchronously referencePart with implementation-defined method
     */
    virtual QFuture<bool> referencePart() = 0;

    virtual bool isReferenced() const = 0;

    /**
     transforms position from part to robot
     */
    virtual position_t transformPartToRobot(const position_t &position) const = 0;
    /**
     transforms position from part to robot
     */
    virtual position_t transformRobotToPart(const position_t &position) const = 0;
};

/**
 * @brief The IPointPairsPartReferencer interface
 *
 * References part with at least three points
 */
class IPointPairsPartReferencer:
        virtual public IPartReferencer
{
public:
    struct point_pair_t
    {
        gp_Vec t;        // translation, part coordinates
        gp_Vec t_robot;  // translation, robot coordinates
    };

    virtual ~IPointPairsPartReferencer() = default;

    virtual void setPointPairs(const std::vector<point_pair_t> &point_pairs) = 0;
};

/**
 * @brief The IFixturePartReferencer interface
 *
 * reference SHAPE_PART according to it position to SHAPE_FIXTURE
 * SHAPE_PART and SHAPE_FIXTURE must be available via scene_provider
 */
class IFixturePartReferencer:
        virtual public ISceneObserver,
        virtual public IPartReferencer
{
public:
    virtual ~IFixturePartReferencer() = default;
};

#endif // IPARTREFERENCER_HPP
