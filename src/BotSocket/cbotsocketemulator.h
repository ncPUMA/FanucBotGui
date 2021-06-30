#ifndef CBOTSOCKETEMULATOR_H
#define CBOTSOCKETEMULATOR_H

#include <QObject>
#include <QFutureWatcher>

#include "cabstractbotsocket.h"
#include <memory>
#include "../if/ipartreferencer.hpp"
#include "../if/irobotmover.hpp"
#include "../if/ipositionreceiver.hpp"
#include "../if/isceneprovider.hpp"

class CBotSocketEmulator :
        public QObject,
        public CAbstractBotSocket,
        public IPositionReceiver,
        public ISceneProvider
{
    Q_OBJECT

public:
    CBotSocketEmulator(bool grip_attached = false, double linear_speed = 1.0, double angular_speed = 90.0);

protected:
    // IPositionReceiver
    void updatePosition(const position_t &pos) override;
    void updateConnectionState(PositionState state) override;

    // ISceneProvider
    const TopoDS_Shape &getShape(ShapeType type) override;

    // CAbstractBotSocket
    void init(const GUI_TYPES::EN_UserActions curAction,
              const std::map <BotSocket::EN_ShapeType, TopoDS_Shape> &shapes) override;
    void shapeTransformChanged(const BotSocket::EN_ShapeType shType) override;
    void usrActionChanged(const GUI_TYPES::EN_UserActions action) override;

private slots:
    void reference_finished();
    void move_finished();

private:
    std::unique_ptr<IPointPairsPartReferencer> point_pair_part_referencer_;
    std::unique_ptr<IFixturePartReferencer> fixture_part_referencer_;
    QFutureWatcher<bool> reference_watcher_;

    std::unique_ptr<IRobotMover> robot_mover_;
    QFutureWatcher<IRobotMover::MOVE_RESULT> move_watcher_;

    bool grip_attached_ = false;
};

#endif // CBOTSOCKETEMULATOR_H
