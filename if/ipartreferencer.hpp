#ifndef IPARTREFERENCER_HPP
#define IPARTREFERENCER_HPP

#include <vector>
#include <future>
#include <gp_Vec.hxx>
#include <gp_Quaternion.hxx>

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
    struct position_t
    {
        gp_Vec t;           // translation
        gp_Quaternion r;    // rotation
    };

    virtual ~IPartReferencer() = default;

    /**
     * @brief referencePart
     * asynchronously referencePart with implementation-defined method
     */
    virtual std::future<bool> referencePart() = 0;

    virtual bool isReferenced() const = 0;

    /**
     transforms position from part to robot
     */
    virtual position_t transformPartToRobot(const position_t &position) const = 0;
};

/**
 * @brief The IPartReferencer interface
 *
 * References part with at least three points
 */
class IPartPointPairsReferencer:
        virtual public IPartReferencer
{
public:
    struct point_pair_t
    {
        gp_Vec t;               // translation, part coordinates
        gp_Quaternion t_robot;  // translation, robot coordinates
    };

    virtual ~IPartPointPairsReferencer() = default;

    virtual void setPointPairs(const std::vector<point_pair_t> &point_pairs) = 0;
};

/**
 * @brief The IPartReferencer interface
 *
 * References part with at least three points
 * reference SHAPE_PART according to it position to SHAPE_FIXTURE
 * SHAPE_PART and SHAPE_FIXTURE must be available via scene_provider
 */
class IPartFixtureReferencer:
        virtual public ISceneObserver,
        virtual public IPartReferencer
{
public:
    virtual ~IPartFixtureReferencer() = default;
};

#endif // IPARTREFERENCER_HPP
