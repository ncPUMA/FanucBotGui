#ifndef EMULATORLASERMOVER_H
#define EMULATORLASERMOVER_H

#include <QTimer>
#include "if/ipositionreceiver.hpp"
#include "if/irobotmover.hpp"
#include "if/ipartreferencer.hpp"

class EmulatorLaserMover:
        public IPositionNotifier,
        public IRobotMover
{
public:
    EmulatorLaserMover(double linear_speed = 1.0, double angular_speed = 15.0);

    void setPositionReceiver(IPositionReceiver *receiver) override;
    void setPartReferencer(IPartReferencer *part_referencer) override;

    QFuture<MOVE_RESULT> moveToPosition(const path_point_t &position) override;
    QFuture<MOVE_RESULT> moveAlongPath(const path_t &path) override;

    void abortMove() override;

    bool isMoving() const;

private:
    bool aborted_ = false;
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
