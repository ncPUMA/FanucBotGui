#ifndef EMULATORLASERMOVER_H
#define EMULATORLASERMOVER_H

#include <atomic>
#include <mutex>
#include <QTimer>
#include "../if/ipositionreceiver.hpp"
#include "../if/irobotmover.hpp"
#include "../if/ipartreferencer.hpp"

class EmulatorLaserMover:
        public IPositionNotifier,
        public IRobotMover
{
public:
    EmulatorLaserMover(float update_frequency = 10, double linear_speed = 1.0, double angular_speed = 90.0);
    EmulatorLaserMover(IPositionReceiver *receiver, IPartReferencer *part_referencer, float update_frequency = 10, double linear_speed = 1.0, double angular_speed = 90.0);

    void setPositionReceiver(IPositionReceiver *receiver) override;
    void setPartReferencer(IPartReferencer *part_referencer) override;

    QFuture<MOVE_RESULT> moveToPosition(const path_point_t &position) override;
    QFuture<MOVE_RESULT> moveAlongPath(const path_t &path) override;

    void abortMove() override;

    bool isMoving() const override;

private:
    bool isGoalEqualCurrent() const;

    mutable std::recursive_mutex mutex_;
    std::atomic_bool aborted_, moving_;
    position_t cur_, goal_;
    double linear_speed_ = 0.0, angular_speed_ = 0.0;
    QTimer position_notify_timer_;

    MOVE_RESULT moveToPositionAsync(const path_point_t &position);
    MOVE_RESULT moveAlongPathAsync(const path_t &position);

    void updatePosition();

    IPositionReceiver *position_receiver_ = nullptr;
    IPartReferencer   *part_referencer_ = nullptr;
};


#endif // EMULATORLASERMOVER_H
