#include <catch2/catch.hpp>
#include <gp_Vec.hxx>

#include "../src/PartReference/pointpairspartreferencer.h"

TEST_CASE( "point pair reference ok", "[point_pairs_part_referencer]" )
{
    std::vector<PointPairsPartReferencer::point_pair_t> train_points[] = {
        {{gp_Vec(0,0,0), gp_Vec(0,0,0)},
         {gp_Vec(0,0,1), gp_Vec(0,0,1)},
         {gp_Vec(0,1,0), gp_Vec(0,1,0)}},

        {{gp_Vec(0,0,0), gp_Vec(0,0,0)},
         {gp_Vec(0,0,1), gp_Vec(0,0,2)},
         {gp_Vec(0,1,0), gp_Vec(0,2,0)},
         {gp_Vec(1,0,0), gp_Vec(2,0,0)}}
    };
    double max_error_train[] = {1e-6, 1e-6};
    int i = GENERATE(range(1, 2));

    PointPairsPartReferencer pr;

    pr.setPointPairs(train_points[i]);

    CHECK_FALSE(pr.isReferenced());

    SECTION("reference") {
        QFuture<bool> ref_result = pr.referencePart();

        ref_result.waitForFinished();

        CHECK(ref_result.result());
        CHECK(pr.isReferenced());

        SECTION("reference_robot_to_part_train")
        {
            for(PointPairsPartReferencer::point_pair_t p : train_points[i])
            {
                gp_Vec t_result = pr.transformRobotToPart({p.t_robot, gp_Quaternion()}).t;
                INFO("t part   " << p.t.X() << ',' << p.t.Y() << ',' << p.t.Z());
                INFO("t robot  " << p.t_robot.X() << ',' << p.t_robot.Y() << ',' << p.t_robot.Z());
                INFO("t result " << t_result.X() << ',' << t_result.Y() << ',' << t_result.Z());
                CHECK(p.t.IsEqual(t_result, max_error_train[i], max_error_train[i]));
            }
        }

        SECTION("reference_part_to_robot_train")
        {
            for(PointPairsPartReferencer::point_pair_t p : train_points[i])
                CHECK(p.t_robot.IsEqual(pr.transformPartToRobot({p.t, gp_Quaternion()}).t, max_error_train[i], max_error_train[i]));
        }
    }
}
