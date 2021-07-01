#include "emulatorlasermover.h"
#include <QtConcurrent/QtConcurrentRun>

EmulatorLaserMover::EmulatorLaserMover(float update_frequency, double linear_speed, double angular_speed):
    aborted_(false),
    moving_(false),
    linear_speed_(linear_speed),
    angular_speed_(angular_speed)
{
    QObject::connect(&position_notify_timer_, &QTimer::timeout, [this](){
        updatePosition();
    });
    position_notify_timer_.start(1000/update_frequency);
}

EmulatorLaserMover::EmulatorLaserMover(IPositionReceiver *receiver, IPartReferencer *part_referencer, float update_frequency, double linear_speed, double angular_speed):
    EmulatorLaserMover(update_frequency, linear_speed, angular_speed)
{
    setPositionReceiver(receiver);
    setPartReferencer(part_referencer);
}

void EmulatorLaserMover::setPositionReceiver(IPositionReceiver *receiver)
{
    std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
    position_receiver_ = receiver;
}
void EmulatorLaserMover::setPartReferencer(IPartReferencer *part_referencer)
{
    std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
    part_referencer_ = part_referencer;
}

QFuture<EmulatorLaserMover::MOVE_RESULT> EmulatorLaserMover::moveToPosition(const path_point_t &position)
{
    return QtConcurrent::run(this, &EmulatorLaserMover::moveToPositionAsync, position);
}

QFuture<EmulatorLaserMover::MOVE_RESULT> EmulatorLaserMover::moveAlongPath(const path_t &path)
{
    return QtConcurrent::run(this, &EmulatorLaserMover::moveAlongPathAsync, path);
}

void EmulatorLaserMover::abortMove()
{
    aborted_ = true;
}

bool EmulatorLaserMover::isMoving() const
{
    return moving_;
}

bool EmulatorLaserMover::isGoalEqualCurrent() const
{
    return cur_.t.IsEqual(goal_.t, gp::Resolution(), gp::Resolution()) && ((cur_.r-goal_.r).Norm() < gp::Resolution());
}

void EmulatorLaserMover::updatePosition()
{
    std::lock_guard<std::recursive_mutex> lock_guard(mutex_);

    position_t diff = {goal_.t - cur_.t, goal_.r - cur_.r};
    if(linear_speed_ > 0.0)
    {
        double max_move = linear_speed_ * position_notify_timer_.interval() / 1000.0;

        diff.t.SetX(std::max(-max_move, std::min(diff.t.X(), max_move)));
        diff.t.SetY(std::max(-max_move, std::min(diff.t.Y(), max_move)));
        diff.t.SetZ(std::max(-max_move, std::min(diff.t.Z(), max_move)));
    }

    if(angular_speed_ > 0.0)
    {
        double max_move = angular_speed_*M_PI / 180 * position_notify_timer_.interval() / 1000.0;

        diff.r.Set(std::max(-max_move, std::min(diff.r.X(), max_move)),
                   std::max(-max_move, std::min(diff.r.Y(), max_move)),
                   std::max(-max_move, std::min(diff.r.Z(), max_move)),
                   std::max(-max_move, std::min(diff.r.W(), max_move)));
    }


    cur_ = {cur_.t + diff.t, cur_.r + diff.r};
    moving_ = (!isGoalEqualCurrent()) && (!aborted_);

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
    if(part_referencer_ != nullptr)
        goal_ = part_referencer_->transformPartToRobot(position.pos);
    else
        goal_ = position.pos;
    moving_ = !isGoalEqualCurrent();
    aborted_ = false;

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

