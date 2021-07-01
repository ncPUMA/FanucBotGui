#include <catch2/catch.hpp>

#include "../src/PartReference/pointpairspartreferencer.h"

TEST_CASE( "point pair reference ok", "[point_pairs_part_referencer]" )
{
    const int TESTS_COUNT = 4;
    std::vector<PointPairsPartReferencer::point_pair_t> train_points[TESTS_COUNT] = {
        {{gp_Vec(0,0,0), gp_Vec(0,0,0)}, // unity
         {gp_Vec(0,0,1), gp_Vec(0,0,1)},
         {gp_Vec(0,1,0), gp_Vec(0,1,0)}},

        {{gp_Vec(0,0,0), gp_Vec(0,0,0)}, // rotation
         {gp_Vec(0,0,1), gp_Vec(0,1,0)},
         {gp_Vec(0,1,0), gp_Vec(1,0,0)},
         {gp_Vec(1,0,0), gp_Vec(0,0,1)}},

        {{gp_Vec(0,0,0), gp_Vec(0,0,0)}, // scale
         {gp_Vec(0,0,1), gp_Vec(0,0,2)},
         {gp_Vec(0,1,0), gp_Vec(0,2,0)},
         {gp_Vec(1,0,0), gp_Vec(2,0,0)}},

        {{gp_Vec(0,0,0), gp_Vec(1e-3,-1e-3,0)}, // scale+noise
         {gp_Vec(0,0,1), gp_Vec(-1e-3,1e-3,2)},
         {gp_Vec(0,1,0), gp_Vec(1e-3,1e-3+2,0)},
         {gp_Vec(1,0,0), gp_Vec(2,-1e-3,1e-3)}}
    };

    std::vector<std::pair<position_t, position_t>> test_points[TESTS_COUNT] = {
        {{{gp_Vec(3,3,3), gp_Quaternion()}, {gp_Vec(3,3,3), gp_Quaternion()}}}, // unity

        {{{gp_Vec(0,0,1), gp_Quaternion(gp_Vec(0,0,-1), 0)},
          {gp_Vec(0,1,0), gp_Quaternion(gp_Vec(0,-1,0), 0)}}}, //rotation

        {{{gp_Vec(3,3,3), gp_Quaternion()}, {gp_Vec(6,6,6), gp_Quaternion()}}}, // scale

        {{{gp_Vec(3,3,3), gp_Quaternion()}, {gp_Vec(6,6,6), gp_Quaternion()}}} // scale + noise
    };
    double max_error[TESTS_COUNT] = {1e-6, 1e-6, 1e-6, 1e-3};
    int i = GENERATE(range(0, 3));

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
                CHECK(p.t.IsEqual(t_result, max_error[i], max_error[i]));
            }
        }

        SECTION("reference_part_to_robot_train")
        {
             for(PointPairsPartReferencer::point_pair_t p : train_points[i])
                 CHECK(p.t_robot.IsEqual(pr.transformPartToRobot({p.t, gp_Quaternion()}).t, max_error[i], max_error[i]));
        }

        SECTION("reference_robot_to_part_test")
        {
            for(std::pair<position_t, position_t> p : test_points[i])
            {
                position_t result = pr.transformRobotToPart(p.second);
                CHECK(p.first.t.IsEqual(result.t, max_error[i], max_error[i]));
                CHECK((p.first.r - result.r).Norm() < max_error[i]);
            }
        }

        SECTION("reference_part_to_robot_test")
        {
            for(std::pair<position_t, position_t> p : test_points[i])
            {
                position_t result = pr.transformPartToRobot(p.first);
                CHECK(p.second.t.IsEqual(result.t, max_error[i], max_error[i]));
                CHECK((p.second.r - result.r).Norm() < max_error[i]);
            }
        }
    }
}
