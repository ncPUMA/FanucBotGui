#include "emulatorlasermover.h"
#include <QtConcurrent/QtConcurrentRun>

int TIMER_MSEC = 100;

EmulatorLaserMover::EmulatorLaserMover(double linear_speed, double angular_speed):
    linear_speed_(linear_speed),
    angular_speed_(angular_speed)
{
    QObject::connect(&position_notify_timer_, &QTimer::timeout, [this](){
        updatePosition();
    });
    position_notify_timer_.start(TIMER_MSEC);
}

void EmulatorLaserMover::setPositionReceiver(IPositionReceiver *receiver)
{
    position_receiver_ = receiver;
}
void EmulatorLaserMover::setPartReferencer(IPartReferencer *part_referencer)
{
    part_referencer_ = part_referencer;
}

QFuture<EmulatorLaserMover::MOVE_RESULT> EmulatorLaserMover::moveToPosition(const path_point_t &position)
{
    return QtConcurrent::run(this, &EmulatorLaserMover::moveToPositionAsync, position);
}

QFuture<EmulatorLaserMover::MOVE_RESULT> EmulatorLaserMover::moveAlongPath(const path_t &path)
{
    return QtConcurrent::run(this, &EmulatorLaserMover::moveAlongPath, path);
}

void EmulatorLaserMover::abortMove()
{
    aborted_ = true;
}

bool EmulatorLaserMover::isMoving() const
{
    return cur_.t.IsEqual(goal_.t, gp::Resolution(), gp::Resolution()) && cur_.r.IsEqual(goal_.r);
}

void EmulatorLaserMover::updatePosition()
{
    position_t diff = {goal_.t - cur_.t, goal_.r - cur_.r};
    if(linear_speed_ > 0.0)
    {
        double max_move = linear_speed_ * TIMER_MSEC / 1000.0;

        goal_.t.SetX(std::max(-max_move, std::min(diff.t.X(), max_move)));
        goal_.t.SetY(std::max(-max_move, std::min(diff.t.Y(), max_move)));
        goal_.t.SetZ(std::max(-max_move, std::min(diff.t.Z(), max_move)));
    }

    if(angular_speed_ > 0.0)
    {
        double max_move = angular_speed_*M_PI / 180 * TIMER_MSEC / 1000.0;

        goal_.r.Set(std::max(-max_move, std::min(diff.r.X(), max_move)),
                    std::max(-max_move, std::min(diff.r.Y(), max_move)),
                    std::max(-max_move, std::min(diff.r.Z(), max_move)),
                    std::max(-max_move, std::min(diff.r.W(), max_move)));
    }


    cur_ = {cur_.t + diff.t, cur_.r + diff.r};

    if(position_receiver_ != nullptr)
    {
        position_t pos = cur_;

        if(part_referencer_ != nullptr)
            pos = part_referencer_->transformRobotToPart(pos);

        position_receiver_->updatePosition(pos);
        position_receiver_->updateConnectionState(IPositionReceiver::POSITION_STATE_ATTACHED);
    }
}

EmulatorLaserMover::MOVE_RESULT EmulatorLaserMover::moveToPositionAsync(const path_point_t &position)
{
    aborted_ = false;
    if(part_referencer_ != nullptr)
        goal_ = part_referencer_->transformPartToRobot(position.pos);
    else
        goal_ = position.pos;

    while(isMoving())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    if(position.timeout_ms > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(position.timeout_ms));
    }
    return aborted_ ? EmulatorLaserMover::MOVE_USER_ABORT : EmulatorLaserMover::MOVE_OK;
}

EmulatorLaserMover::MOVE_RESULT EmulatorLaserMover::moveAlongPathAsync(const path_t &path)
{
    for(const path_point_t &p : path)
    {
        EmulatorLaserMover::MOVE_RESULT res = moveToPositionAsync(p);
        if(res != MOVE_OK)
            return res;
    }
    return MOVE_OK;
}

