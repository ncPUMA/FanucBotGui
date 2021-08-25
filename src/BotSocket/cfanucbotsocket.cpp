#include "cfanucbotsocket.h"

#include <QTimer>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QStringList>

#include <Precision.hxx>

#include "../PartReference/pointpairspartreferencer.h"
#include "../log/loguru.hpp"

CFanucBotSocket::CFanucBotSocket() :
    CAbstractBotSocket(),
    lastTaskDelay(0),
    calibWaitCounter(0)
{
    VLOG_CALL;


    QSettings settings("fanuc.ini", QSettings::IniFormat);

    if(settings.contains("world2user") && settings.contains("user2world"))
    {
        QList<QVariant> w2u_v = settings.value("world2user").toList();
        QList<QVariant> u2w_v = settings.value("user2world").toList();

        if(w2u_v.size() >= 12 && u2w_v.size() >= 12)
        {
            QList<double> w2u_d, u2w_d;
            for(QVariant v : w2u_v)
                w2u_d.push_back(v.toDouble());
            for(QVariant v : u2w_v)
                u2w_d.push_back(v.toDouble());

            world2user_.SetValues(w2u_d[0],w2u_d[1],w2u_d[2],w2u_d[3],
                                  w2u_d[4],w2u_d[5],w2u_d[6],w2u_d[7],
                                  w2u_d[8],w2u_d[9],w2u_d[10],w2u_d[11]);
            user2world_.SetValues(u2w_d[0],u2w_d[1],u2w_d[2],u2w_d[3],
                                  u2w_d[4],u2w_d[5],u2w_d[6],u2w_d[7],
                                  u2w_d[8],u2w_d[9],u2w_d[10],u2w_d[11]);
        }
    }

    connect(&fanuc_state_, &FanucStateSocket::xyzwpr_position_received, this, &CFanucBotSocket::updatePosition);

    connect(&fanuc_state_, &FanucStateSocket::connection_state_changed, this, &CFanucBotSocket::updateConnectionState);

//    connect(&fanuc_state_, &FanucStateSocket::status_received, [&](bool moving, bool ready_to_move, bool error){
//        moving_ = moving;
//    });

    connect(&fanuc_relay_, &FanucRelaySocket::connection_state_changed, this, &CFanucBotSocket::updateConnectionState);
    // TODO: another signal / monitor position
    connect(&fanuc_relay_, &FanucRelaySocket::trajectory_enqueue_finished, this, [&](){
        completePath(BotSocket::ENWR_OK);
    });
    connect(&fanuc_relay_, &FanucRelaySocket::trajectory_xyzwpr_point_enqueue_fail, this, [&](){
        completePath(BotSocket::ENWR_ERROR);
    });
}


xyzwpr_data transform(const xyzwpr_data &p, const gp_Trsf &t)
{
    gp_Trsf pos_src;
    gp_Quaternion r;

    r.SetEulerAngles(gp_Extrinsic_XYZ, p.xyzwpr[3] * M_PI/180, p.xyzwpr[4] * M_PI/180, p.xyzwpr[5] * M_PI/180);
    pos_src.SetRotation(r);
    pos_src.SetTranslationPart(gp_Vec(p.xyzwpr[0], p.xyzwpr[1], p.xyzwpr[2]));

    gp_Trsf pos_result = t * pos_src;

    xyzwpr_data p_result = p;
    p_result.xyzwpr[0] = pos_result.TranslationPart().X();
    p_result.xyzwpr[1] = pos_result.TranslationPart().Y();
    p_result.xyzwpr[2] = pos_result.TranslationPart().Z();

    r = pos_result.GetRotation().Normalized();

    r.GetEulerAngles(gp_Extrinsic_XYZ, p_result.xyzwpr[3], p_result.xyzwpr[4], p_result.xyzwpr[5]);

    p_result.xyzwpr[3] *= 180/M_PI, p_result.xyzwpr[4] *= 180/M_PI, p_result.xyzwpr[5] *= 180/M_PI;

    return p_result;
}

BotSocket::SBotPosition xyzwpr2botposition(const xyzwpr_data &pos, const gp_Trsf &world2user)
{
    xyzwpr_data p = transform(pos, world2user);
    return BotSocket::SBotPosition(p.xyzwpr[0], p.xyzwpr[1], p.xyzwpr[2], p.xyzwpr[3], p.xyzwpr[4], p.xyzwpr[5]);
}

xyzwpr_data botposition2xyzwpr(const GUI_TYPES::SVertex &pos, const GUI_TYPES::SRotationAngle &angle, const gp_Trsf &user2world)
{
    xyzwpr_data p;
    p.xyzwpr = {pos.x, pos.y, pos.z, angle.x, angle.y, angle.z};
    return transform(p, user2world);
}

void CFanucBotSocket::updatePosition(const xyzwpr_data &pos)
{
    laserHeadPositionChanged(xyzwpr2botposition(pos, world2user_));
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

void CFanucBotSocket::completePath(const BotSocket::EN_WorkResult result)
{
    if (result != BotSocket::ENWR_OK)
    {
        curTask.clear();
        tasksComplete(result);
        return;
    }

    if (curTask.empty())
    {
        tasksComplete(result); //result == BotSocket::ENWR_OK
        return;
    }

    GUI_TYPES::STaskPoint &p = curTask.front();
    if (p.bNeedCalib)
    {
        p.bNeedCalib = false;
        QFile calibResFile("calib_result.txt");
        if (calibResFile.exists())
            calibResFile.remove();

        makePartSnapshot("snapshot.bmp");

        calibWaitCounter = 0;
        QTimer::singleShot(1000, this, &CFanucBotSocket::slCalibWaitTimeout);
    }
    else if(lastTaskDelay > 0)
    {
        QTimer::singleShot(lastTaskDelay, this, [&]() {
            completePath(BotSocket::ENWR_OK);
        });
        lastTaskDelay = 0;
    }
    else
    {
        std::vector<xyzwpr_data> path;
        auto it = curTask.begin();
        LOG_F(INFO, "Task %d: %f %f %f %f %f %f",
              p.taskType,
              p.globalPos.x, p.globalPos.y, p.globalPos.z,
              p.angle.x, p.angle.y, p.angle.z);
        path.emplace_back(botposition2xyzwpr(p.globalPos, p.angle, user2world_));
        lastTaskDelay = static_cast <int> (p.delay * 1000.);
        it = curTask.erase(it);
        fanuc_relay_.move_trajectory(path);
    }
}

void CFanucBotSocket::calibFinish(const gp_Vec &delta)
{
    if (!delta.IsEqual(gp_Vec(), Precision::Confusion(), Precision::Angular()))
    {
        //Current task correction
        gp_Trsf lHeadPos = getShapeTransform(BotSocket::ENST_LSRHEAD);
        gp_Quaternion quatLsrHead = lHeadPos.GetRotation();
        const gp_Vec rotatedDelta = quatLsrHead.Multiply(delta);
        for (auto &p : curTask)
        {
            p.globalPos.x += rotatedDelta.X();
            p.globalPos.y += rotatedDelta.Y();
            p.globalPos.z += rotatedDelta.Z();
        }
        snapshotCalibrationDataRecieved(rotatedDelta);
    }
    completePath(BotSocket::ENWR_OK);
}

void CFanucBotSocket::slCalibWaitTimeout()
{
    static const int CALIB_ATTEMP_COUNT = 3;
    QFile calibResFile("calib_result.txt");
    if (!calibResFile.exists())
    {
        if (calibWaitCounter < CALIB_ATTEMP_COUNT)
        {
            ++calibWaitCounter;
            QTimer::singleShot(1000, this, &CFanucBotSocket::slCalibWaitTimeout);
        }
        else
        {
            calibWaitCounter = 0;
            calibFinish(gp_Vec());
        }
    }
    else
    {
        gp_Vec delta;
        if (calibResFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&calibResFile);
            QStringList line = in.readLine().split(";");
            calibResFile.close();

            if (line.size() > 2)
                delta = gp_Vec(line.at(0).toDouble(),
                               line.at(1).toDouble(),
                               line.at(2).toDouble());
        }
        calibFinish(delta);
    }
}

BotSocket::EN_CalibResult CFanucBotSocket::execCalibration(const std::vector<GUI_TYPES::SCalibPoint> &points)
{
    VLOG_CALL;

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

void CFanucBotSocket::startTasks(const std::vector<GUI_TYPES::SPathPoint> &,
                                 const std::vector<GUI_TYPES::STaskPoint> &taskPoints)
{
    VLOG_CALL;

    /**
      Rotation extracting example

    for(const GUI_TYPES::STaskPoint &stpnt : taskPoints)
    {
        //normal represent as the ZAxis
        gp_Dir zDir(stpnt.normal.x, stpnt.normal.y, stpnt.normal.z);

        //normal rotation angles from global coord system
        gp_Quaternion normal(gp_Vec(gp_Dir(0., 0., 1.)), gp_Vec(zDir));

        //delta rotation from global coord system
        gp_Quaternion delta;
        delta.SetEulerAngles(gp_Extrinsic_XYZ,
                             stpnt.angle.x * M_PI / 180.,
                             stpnt.angle.y * M_PI / 180.,
                             stpnt.angle.z * M_PI / 180.);

        //final rotation from global coord system
        gp_Quaternion finalRotationZAxis = normal * delta;
    }
    */

//    std::vector<xyzwpr_data> path;
//    path.reserve(taskPoints.size());
//    for(GUI_TYPES::STaskPoint p : taskPoints)
//    {
//        LOG_F(INFO, "Task %d: %f %f %f %f %f %f", p.taskType, p.globalPos.x, p.globalPos.y, p.globalPos.z, p.angle.x, p.angle.y, p.angle.z);
//        if(p.taskType == GUI_TYPES::ENBTT_MOVE)
//            path.emplace_back(botposition2xyzwpr(p.globalPos, p.angle, user2world_));
//    }

//    fanuc_relay_.move_trajectory(path);
    curTask = taskPoints;
    completePath(BotSocket::ENWR_OK);
}

void CFanucBotSocket::stopTasks()
{
    VLOG_CALL;
    curTask.clear();
    fanuc_relay_.stop();
}

void CFanucBotSocket::shapeTransformChanged(const BotSocket::EN_ShapeType)
{}
