#include <catch2/catch.hpp>

#include "../src/PartReference/pointpairspartreferencer.h"

TEST_CASE( "Unity reference ok", "[point_pairs_part_referencer]" )
{
    PointPairsPartReferencer pr;

    pr.setPointPairs({{gp_Vec(0,0,0), gp_Vec(0,0,0)}});

    CHECK_FALSE(pr.isReferenced());

//    SECTION("referenc") {
//    }
}
