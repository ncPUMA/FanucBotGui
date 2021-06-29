#ifndef SIMPLELASERMOVER_H
#define SIMPLELASERMOVER_H

#include "../if/ipositionreceiver.hpp"
#include "../if/irobotmover.hpp"
#include "../if/ipartreferencer.hpp"
#include "../BotSocket/fanucsocket.h"

class SimpleLaserMover:
        public IPositionNotifier,
        public IRobotMover
{
public:
    SimpleLaserMover();

    void setPositionReceiver(IPositionReceiver *receiver) override;
    void setPartReferencer(IPartReferencer *part_referencer) override;

    QFuture<MOVE_RESULT> moveToPosition(const path_point_t &position) override;
    QFuture<MOVE_RESULT> moveAlongPath(const path_t &path) override;
    void abortMove() override;

private:
    FanucSocket fanuc_socket_;

    void updatePosition(struct FanucSocket::position pos);
    IPositionReceiver::PositionState socketState() const;

    IPositionReceiver *position_receiver_ = nullptr;
    IPartReferencer   *part_referencer_ = nullptr;
};

#endif // SIMPLELASERMOVER_H
