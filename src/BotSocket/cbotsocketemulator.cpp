#include "cbotsocketemulator.h"

#include <QTimer>

#include "../RobotMovers/emulatorlasermover.h"
#include "../PartReference/pointpairspartreferencer.h"

CBotSocketEmulator::CBotSocketEmulator(bool grip_attached, float update_frequency, double linear_speed, double angular_speed):
    CAbstractBotSocket(),
    point_pair_part_referencer_(std::make_unique<PointPairsPartReferencer>()),
    robot_mover_(std::move(std::make_unique<EmulatorLaserMover>(this, point_pair_part_referencer_.get(), update_frequency, linear_speed, angular_speed))),
    grip_attached_(grip_attached)
{
    connect(&move_watcher_, &QFutureWatcher<IRobotMover::MOVE_RESULT>::finished, this, &CBotSocketEmulator::move_finished);
    connect(&reference_watcher_, &QFutureWatcher<bool>::finished, this, &CBotSocketEmulator::reference_finished);
}

BotSocket::SBotPosition position2botposition(const position_t &pos)
{
    //TODO: quaternion in sbotposition
    GUI_TYPES::SRotationAngle r;
    pos.r.GetEulerAngles(gp_Extrinsic_XYZ, r.x, r.y, r.z);

    return BotSocket::SBotPosition(pos.t.X(), pos.t.Y(), pos.t.Z(), r.x, r.y, r.z);
}

position_t botposition2position(const GUI_TYPES::SVertex &pos, const GUI_TYPES::SRotationAngle &angle)
{
    //TODO: quaternion in sbotposition
    position_t p = {gp_Vec{pos.x, pos.y, pos.z}, gp_Quaternion()};
    p.r.SetEulerAngles(gp_Extrinsic_XYZ, angle.x, angle.y, angle.z);
    return p;
}

void CBotSocketEmulator::updatePosition(const position_t &pos)
{
    if(grip_attached_)
    {
        gripPositionChanged(position2botposition(pos));
    }
    else
    {
        laserHeadPositionChanged(position2botposition(pos));
    }
}

void CBotSocketEmulator::updateConnectionState(PositionState state)
{
    // TODO: single enum type
    std::map<IPositionReceiver::PositionState, BotSocket::EN_BotState> state_conv {
        {POSITION_STATE_FALL,            BotSocket::ENBS_FALL},
        {POSITION_STATE_NOT_ATTACHED,    BotSocket::ENBS_NOT_ATTACHED},
        {POSITION_STATE_ATTACHED,        BotSocket::ENBS_ATTACHED}
    };
    socketStateChanged(state_conv[state]);
}

const TopoDS_Shape &CBotSocketEmulator::getShape(ShapeType type)
{
    // TODO: single enum type
    std::map<ISceneProvider::ShapeType, BotSocket::EN_ShapeType> shape_type_conv {
        {SHAPE_PART,      BotSocket::ENST_PART},
        {SHAPE_GRIP,      BotSocket::ENST_GRIP},
        {SHAPE_LASERHEAD, BotSocket::ENST_LSRHEAD},
        {SHAPE_FIXTURE,   BotSocket::ENST_DESK}
    };

    return CAbstractBotSocket::getShape(shape_type_conv[type]);
}

BotSocket::EN_CalibResult CBotSocketEmulator::execCalibration(const std::vector<GUI_TYPES::SCalibPoint> &points)
{
    if(robot_mover_ && robot_mover_->isMoving())
        robot_mover_->abortMove();

    // TODO: distinguish calibration by point pairs/fixture
    std::vector<IPointPairsPartReferencer::point_pair_t> point_pairs;

    BotSocket::EN_CalibResult result = BotSocket::ENCR_FALL;
    if(points.size() > 3)
    {
        point_pairs.reserve(points.size());
        for(GUI_TYPES::SCalibPoint p : points)
            point_pairs.emplace_back(IPointPairsPartReferencer::point_pair_t{
                                         gp_Vec(p.globalPos.x, p.globalPos.y, p.globalPos.z),
                                         gp_Vec(p.botPos.x, p.botPos.y, p.botPos.z)
                                     });

        point_pair_part_referencer_->setPointPairs(point_pairs);

        QFuture<bool> res = point_pair_part_referencer_->referencePart();
        reference_watcher_.setFuture(res);
        result = BotSocket::ENCR_OK;
    }
    return result;
}

void CBotSocketEmulator::startTasks(const std::vector<GUI_TYPES::STaskPoint> &points)
{
    if(robot_mover_ && robot_mover_->isMoving())
        robot_mover_->abortMove();

    IRobotMover::path_t path;

    path.reserve(points.size());
    for(GUI_TYPES::STaskPoint p : points)
        path.emplace_back(IRobotMover::path_point_t{botposition2position(p.globalPos, p.angle), 0, ""});

    QFuture<IRobotMover::MOVE_RESULT> res = robot_mover_->moveAlongPath(path);
    move_watcher_.setFuture(res);
}

void CBotSocketEmulator::stopTasks()
{
    if(robot_mover_ && robot_mover_->isMoving())
        robot_mover_->abortMove();
}

void CBotSocketEmulator::shapeTransformChanged(const BotSocket::EN_ShapeType)
{}

void CBotSocketEmulator::reference_finished()
{
    bool reference_ok = reference_watcher_.future().result();
    qDebug() << "BotSocketEmulator::reference_finished. Result: " << reference_ok;
    // TODO: notify GUI about reference completion
}

void CBotSocketEmulator::move_finished()
{
    IRobotMover::MOVE_RESULT move_ok = move_watcher_.future().result();
    std::map<IRobotMover::MOVE_RESULT, QString> move_result_map = {
        {IRobotMover::MOVE_OK, "MOVE_OK"},
        {IRobotMover::MOVE_USER_ABORT, "MOVE_USER_ABORT"},
        {IRobotMover::MOVE_FAILURE, "MOVE_FAILURE"},
        {IRobotMover::MOVE_UNACHIEVABLE, "MOVE_UNACHIEVABLE"}
    };
    const BotSocket::EN_WorkResult res = ((move_result_map[move_ok] == IRobotMover::MOVE_OK) ||
                                          (move_result_map[move_ok] == IRobotMover::MOVE_USER_ABORT))
                                           ? BotSocket::ENWR_OK
                                           : BotSocket::ENWR_ERROR;
    tasksComplete(res);
}
