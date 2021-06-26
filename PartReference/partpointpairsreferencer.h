#ifndef PARTPOINTPAIRSREFERENCER_H
#define PARTPOINTPAIRSREFERENCER_H

#include "../if/ipartreferencer.hpp"

class PartPointPairsReferencer:
        public IPartPointPairsReferencer
{
public:
    PartPointPairsReferencer();

    void setPointPairs(const std::vector<point_pair_t> &point_pairs) override;
    std::future<bool> referencePart() override;

    position_t transformPartToRobot(const position_t &position) const override;
};


#endif // PARTPOINTPAIRSREFERENCER_H
