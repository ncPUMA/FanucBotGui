#include "pointpairspartreferencer.h"
#include <QtConcurrent/QtConcurrentRun>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <sstream>
#include "../log/loguru.hpp"

PointPairsPartReferencer::PointPairsPartReferencer()
{

}

void PointPairsPartReferencer::setPointPairs(const std::vector<point_pair_t> &point_pairs)
{
    point_pairs_ = point_pairs;
}

bool PointPairsPartReferencer::isReferenced() const
{
    return reference_ok_;
}

bool PointPairsPartReferencer::referencePart()
{
    VLOG_CALL;
    if(point_pairs_.size() < 3)
        return false;

    const bool with_scaling = true;
    const double max_error = 10.0;

    transform_part_to_robot_ = gp_Trsf();
    transform_robot_to_part_ = gp_Trsf();
    reference_ok_ = false;

    Eigen::Matrix<Standard_Real, 3, Eigen::Dynamic> cloud_model (3, point_pairs_.size());
    Eigen::Matrix<Standard_Real, 3, Eigen::Dynamic> cloud_robot (3, point_pairs_.size());

    for (size_t i = 0; i < point_pairs_.size(); ++i)
    {
        cloud_model (0, i) = point_pairs_[i].t.X();
        cloud_model (1, i) = point_pairs_[i].t.Y();
        cloud_model (2, i) = point_pairs_[i].t.Z();
        cloud_robot (0, i) = point_pairs_[i].t_robot.X();
        cloud_robot (1, i) = point_pairs_[i].t_robot.Y();
        cloud_robot (2, i) = point_pairs_[i].t_robot.Z();
        LOG_F(INFO, "(%f,%f,%f) <-> (%f,%f,%f)", cloud_model (0, i), cloud_model (1, i), cloud_model (2, i),
                                                 cloud_robot (0, i), cloud_robot (1, i), cloud_robot (2, i));
    }

    Eigen::Matrix4d model2robot = Eigen::umeyama (cloud_model, cloud_robot, with_scaling);
    Eigen::Matrix4d robot2model = model2robot.inverse();

    Eigen::Matrix<Standard_Real, 3, Eigen::Dynamic> error_matrix = (model2robot * cloud_model.colwise().homogeneous() - cloud_robot.colwise().homogeneous()).topRows<3>();
    double error = error_matrix.norm();

    LOG_F(INFO, "PointPairsPartReferencer: error is %f (max_error is %f)", error, max_error);

    if(error > max_error)
        return false;

    transform_part_to_robot_.SetValues(model2robot(0,0), model2robot(0,1), model2robot(0,2), model2robot(0,3),
                                       model2robot(1,0), model2robot(1,1), model2robot(1,2), model2robot(1,3),
                                       model2robot(2,0), model2robot(2,1), model2robot(2,2), model2robot(2,3));
    transform_robot_to_part_.SetValues(robot2model(0,0), robot2model(0,1), robot2model(0,2), robot2model(0,3),
                                       robot2model(1,0), robot2model(1,1), robot2model(1,2), robot2model(1,3),
                                       robot2model(2,0), robot2model(2,1), robot2model(2,2), robot2model(2,3));

    std::stringstream ss;
    ss << model2robot;
    LOG_F(INFO, "model2robot: %s", ss.str().c_str());
    ss.str("");
    ss.clear();
    ss << robot2model;
    LOG_F(INFO, "robot2model: %s", ss.str().c_str());

    reference_ok_ = true;
    bool res = true;
    //check transform internal matrix 3x4 by Nan value
    for(Standard_Integer i = 1; i < 4; ++i)
        for(Standard_Integer j = 1; j < 5; ++j)
        {
            res = !(std::isnan(transform_part_to_robot_.Value(i, j)) ||
                    std::isnan(transform_robot_to_part_.Value(i, j)));
            if (!res)
                break;
        }

    return res;
}

PointPairsPartReferencer::position_t PointPairsPartReferencer::transformPartToRobot(const position_t &position) const
{
    gp_Trsf pos_src;
    pos_src.SetRotation(position.r);
    pos_src.SetTranslationPart(position.t);

    gp_Trsf pos_result = transform_part_to_robot_ * pos_src;
    return {pos_result.TranslationPart(), pos_result.GetRotation().Normalized()};
}

PointPairsPartReferencer::position_t PointPairsPartReferencer::transformRobotToPart(const position_t &position) const
{
    gp_Trsf pos_src;
    pos_src.SetRotation(position.r);
    pos_src.SetTranslationPart(position.t);

    gp_Trsf pos_result = transform_robot_to_part_ * pos_src;
    return {pos_result.TranslationPart(), pos_result.GetRotation().Normalized()};
}

gp_Trsf PointPairsPartReferencer::getPartToRobotTransformation() const
{
    return transform_part_to_robot_;
}

gp_Trsf PointPairsPartReferencer::getRobotToPartTransformation() const
{
    return transform_robot_to_part_;
}
