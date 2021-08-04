#ifndef POINTPAIRSPARTREFERENCER_H
#define POINTPAIRSPARTREFERENCER_H

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Quaternion.hxx>
#include <vector>

class PointPairsPartReferencer
{
public:
    struct position_t
    {
        gp_Vec t;           // translation
        gp_Quaternion r;    // rotation
    };

    struct point_pair_t
    {
        gp_Vec t;        // translation, part coordinates
        gp_Vec t_robot;  // translation, robot coordinates
    };

    PointPairsPartReferencer();

    void setPointPairs(const std::vector<point_pair_t> &point_pairs);
    bool referencePart();

    bool isReferenced() const;

    position_t transformPartToRobot(const position_t &position) const;
    position_t transformRobotToPart(const position_t &position) const;

    gp_Trsf getPartToRobotTransformation() const;
    gp_Trsf getRobotToPartTransformation() const;

private:
    std::vector<point_pair_t> point_pairs_;
    gp_Trsf transform_part_to_robot_, transform_robot_to_part_;
    bool reference_ok_ = false;
};


#endif // POINTPAIRSPARTREFERENCER_H
