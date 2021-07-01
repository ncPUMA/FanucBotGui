#include "cbotsocketemulator.h"

#include <QTimer>

#include "../RobotMovers/emulatorlasermover.h"
#include "../PartReference/pointpairspartreferencer.h"

CBotSocketEmulator::CBotSocketEmulator(bool grip_attached, double linear_speed, double angular_speed):
    CAbstractBotSocket(),
    point_pair_part_referencer_(std::make_unique<PointPairsPartReferencer>()),
    robot_mover_(std::move(std::make_unique<EmulatorLaserMover>(this, point_pair_part_referencer_.get(), linear_speed, angular_speed))),
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

position_t botposition2position(const BotSocket::SBotPosition &bpos)
{
    //TODO: quaternion in sbotposition
    position_t pos = {gp_Vec{bpos.globalPos.x, bpos.globalPos.y, bpos.globalPos.z}, gp_Quaternion()};
    pos.r.SetEulerAngles(gp_Extrinsic_XYZ, bpos.globalRotation.x, bpos.globalRotation.y, bpos.globalRotation.z);
    return pos;
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
    std::map<IPositionReceiver::PositionState, BotSocket::TBotState> state_conv {
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

void CBotSocketEmulator::shapeTransformChanged(const BotSocket::EN_ShapeType)
{}

void CBotSocketEmulator::uiStateChanged(const GUI_TYPES::EN_UiStates state)
{
    if(state == GUI_TYPES::ENUS_CALIBRATION && point_pair_part_referencer_)
    {
        // TODO: distinguish calibration by point pairs/fixture

        std::vector<IPointPairsPartReferencer::point_pair_t> point_pairs;
        // FIXME: fill point_pairs
        point_pair_part_referencer_->setPointPairs(point_pairs);

        QFuture<bool> res = point_pair_part_referencer_->referencePart();
        reference_watcher_.setFuture(res);
    }
    else if(state == GUI_TYPES::ENUS_BOT_WORKED && robot_mover_)
    {
        // TODO: distinguish movement to point/along path
        IRobotMover::path_point_t p;
        // FIXME: fill path_point_t
        QFuture<IRobotMover::MOVE_RESULT> res = robot_mover_->moveToPosition(p);
        move_watcher_.setFuture(res);
    }
}

void CBotSocketEmulator::reference_finished()
{
    bool reference_ok = reference_watcher_.future().result();
    qDebug() << "BotSocketEmulator::reference_finished. Result: " << reference_ok;
    // TODO: notify GUI about reference completion
}

void CBotSocketEmulator::move_finished()
{
    IRobotMover::MOVE_RESULT move_ok = move_watcher_.future().result();
    qDebug() << "BotSocketEmulator::reference_finished. Result: " << move_ok;
    // TODO: notify GUI about move finished
}
