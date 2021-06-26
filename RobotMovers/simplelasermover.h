#ifndef SIMPLELASERMOVER_H
#define SIMPLELASERMOVER_H

#include "if/ipositionreceiver.hpp"
#include "if/irobotmover.hpp"

class SimpleLaserMover:
        public IPositionNotifier,
        public IRobotMover
{
public:
    SimpleLaserMover();

    void setPositionReceiver(IPositionReceiver *receiver) override;

    std::future<MOVE_RESULT> moveToPosition(const path_point_t &position) override;
    std::future<MOVE_RESULT> moveAlongPath(const path_t &path) override;
    void abortMove() override;
};

#endif // SIMPLELASERMOVER_H
