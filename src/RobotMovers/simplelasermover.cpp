#include "simplelasermover.h"

SimpleLaserMover::SimpleLaserMover()
{
    // connect signals so translateModel/rotateModel are called
    QObject::connect(&fanuc_socket_, &FanucSocket::position_received, [&](struct FanucSocket::position pos){
        updatePosition(pos);
    });
    QObject::connect(&fanuc_socket_, &FanucSocket::connection_state_changed, [&](bool){
        if(position_receiver_)
            position_receiver_->updateConnectionState(socketState());
    });
    QObject::connect(&fanuc_socket_, &FanucSocket::attached_changed, [&](bool){
        if(position_receiver_)
            position_receiver_->updateConnectionState(socketState());
    });
}

void SimpleLaserMover::setPositionReceiver(IPositionReceiver *receiver)
{
    position_receiver_ = receiver;
}

void SimpleLaserMover::setPartReferencer(IPartReferencer *part_referencer)
{
    part_referencer_ = part_referencer;
}

QFuture<IRobotMover::MOVE_RESULT> SimpleLaserMover::moveToPosition(const path_point_t &)
{
    return QFuture<IRobotMover::MOVE_RESULT>();
}

QFuture<IRobotMover::MOVE_RESULT> SimpleLaserMover::moveAlongPath(const path_t &)
{
    return QFuture<IRobotMover::MOVE_RESULT>();
}

void SimpleLaserMover::abortMove()
{

}

void SimpleLaserMover::updatePosition(struct FanucSocket::position pos)
{
    if(position_receiver_)
    {
        position_t p;
        p.t = {pos.x, pos.y, pos.z};
        p.r.SetEulerAngles(gp_Extrinsic_XYZ, pos.w * M_PI/180, pos.p * M_PI/180, pos.r * M_PI/180);
        if(part_referencer_)
            p = part_referencer_->transformRobotToPart(p);
        position_receiver_->updatePosition(p);
    }
}

IPositionReceiver::PositionState SimpleLaserMover::socketState() const
{
    if(fanuc_socket_.connected())
        return fanuc_socket_.attached() ? IPositionReceiver::POSITION_STATE_ATTACHED : IPositionReceiver::POSITION_STATE_NOT_ATTACHED;
    else
        return IPositionReceiver::POSITION_STATE_FALL;
}
