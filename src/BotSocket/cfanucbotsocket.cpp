#include "cfanucbotsocket.h"

#include <QTimer>

#include "../PartReference/pointpairspartreferencer.h"

CFanucBotSocket::CFanucBotSocket()
{
    connect(&fanuc_state_, &FanucStateSocket::xyzwpr_position_received, this, &CFanucBotSocket::updatePosition);

    connect(&fanuc_state_, &FanucStateSocket::connection_state_changed, this, &CFanucBotSocket::updateConnectionState);

//    connect(&fanuc_state_, &FanucStateSocket::status_received, [&](bool moving, bool ready_to_move, bool error){
//        moving_ = moving;
//    });

    connect(&fanuc_relay_, &FanucRelaySocket::connection_state_changed, this, &CFanucBotSocket::updateConnectionState);
    // TODO: another signal / monitor position
    connect(&fanuc_relay_, &FanucRelaySocket::trajectory_enqueue_finished, this, [&](){
        tasksComplete(BotSocket::ENWR_OK);
    });
    connect(&fanuc_relay_, &FanucRelaySocket::trajectory_xyzwpr_point_enqueue_fail, this, [&](){
        tasksComplete(BotSocket::ENWR_ERROR);
    });
}

BotSocket::SBotPosition xyzwpr2botposition(const xyzwpr_data &pos)
{
    return BotSocket::SBotPosition(pos.xyzwpr[0], pos.xyzwpr[1], pos.xyzwpr[2], pos.xyzwpr[3], pos.xyzwpr[4], pos.xyzwpr[5]);
}

xyzwpr_data botposition2xyzwpr(const GUI_TYPES::SVertex &pos, const GUI_TYPES::SRotationAngle &angle)
{
    xyzwpr_data p;
    p.xyzwpr = {pos.x, pos.y, pos.z, angle.x, angle.y, angle.z};
    return p;
}

void CFanucBotSocket::updatePosition(const xyzwpr_data &pos)
{
    gripPositionChanged(xyzwpr2botposition(pos));
}

void CFanucBotSocket::prepare(const std::vector <GUI_TYPES::STaskPoint> &)
{
    prepareComplete(BotSocket::EN_PrepareResult::ENPR_OK);
}

void CFanucBotSocket::updateConnectionState()
{
    bool ok = fanuc_state_.connected() && fanuc_relay_.connected();
    socketStateChanged(ok ? BotSocket::ENBS_NOT_ATTACHED
                          : BotSocket::ENBS_FALL);
}

BotSocket::EN_CalibResult CFanucBotSocket::execCalibration(const std::vector<GUI_TYPES::SCalibPoint> &points)
{
    BotSocket::EN_CalibResult result = BotSocket::ENCR_FALL;
    if(points.size() >= 3)
    {
        std::vector<PointPairsPartReferencer::point_pair_t> point_pairs;
        point_pairs.reserve(points.size());
        for(GUI_TYPES::SCalibPoint p : points) {
            point_pairs.emplace_back(PointPairsPartReferencer::point_pair_t{
                                         gp_Vec(p.globalPos.x, p.globalPos.y, p.globalPos.z),
                                         gp_Vec(p.botPos.x, p.botPos.y, p.botPos.z)
                                     });
        }

        PointPairsPartReferencer point_pair_part_referencer;
        point_pair_part_referencer.setPointPairs(point_pairs);

        bool ok = point_pair_part_referencer.referencePart();
        if(!ok)
            return BotSocket::ENCR_FALL;

        gp_Trsf transform = point_pair_part_referencer.getPartToRobotTransformation();

        // This one is simpler, but cannot change values in GUI settings
//        CAbstractBotSocket::shapeTransformChanged(BotSocket::ENST_PART, transform);

        gp_Vec translation = transform.TranslationPart();
        gp_Quaternion rotation = transform.GetRotation();

        GUI_TYPES::SRotationAngle r;
        rotation.GetEulerAngles(gp_Extrinsic_XYZ, r.x, r.y, r.z);
        r.x *= 180. / M_PI;
        r.y *= 180. / M_PI;
        r.z *= 180. / M_PI;

        // TODO: update part model here
        BotSocket::SBotPosition part_position(translation.X(), translation.Y(), translation.Z(), r.x, r.y, r.z);

        shapeCalibrationChanged(BotSocket::ENST_PART, part_position);

        return BotSocket::ENCR_OK;
    }
    return result;
}

void CFanucBotSocket::startTasks(const std::vector<GUI_TYPES::STaskPoint> &points)
{
    std::vector<xyzwpr_data> path;
    path.reserve(points.size());
    for(GUI_TYPES::STaskPoint p : points)
    {
        if(p.taskType == GUI_TYPES::ENBTT_MOVE)
            path.emplace_back(botposition2xyzwpr(p.globalPos, p.angle));
    }

    fanuc_relay_.move_trajectory(path);
}

void CFanucBotSocket::stopTasks()
{
    fanuc_relay_.stop();
}

void CFanucBotSocket::shapeTransformChanged(const BotSocket::EN_ShapeType)
{}
