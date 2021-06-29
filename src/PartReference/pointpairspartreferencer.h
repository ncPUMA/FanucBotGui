#ifndef POINTPAIRSPARTREFERENCER_H
#define POINTPAIRSPARTREFERENCER_H

#include <gp_Trsf.hxx>
#include "../if/ipartreferencer.hpp"

class PointPairsPartReferencer:
        public IPointPairsPartReferencer
{
public:
    PointPairsPartReferencer();

    void setPointPairs(const std::vector<point_pair_t> &point_pairs) override;
    QFuture<bool> referencePart() override;

    bool isReferenced() const override;

    position_t transformPartToRobot(const position_t &position) const override;
    position_t transformRobotToPart(const position_t &position) const override;

private:
    bool referencePartAsync();

    std::vector<point_pair_t> point_pairs_;
    gp_Trsf transform_part_to_robot_, transform_robot_to_part_;
    bool reference_ok_ = false;
};


#endif // POINTPAIRSPARTREFERENCER_H
