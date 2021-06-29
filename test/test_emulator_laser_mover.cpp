#include <catch2/catch.hpp>
#include <QSignalSpy>
#include <QFutureWatcher>

#include "../src/RobotMovers/emulatorlasermover.h"

class DebugPositionReceiver:
        public IPositionReceiver
{
    void updatePosition(const position_t &pos)
    {
        UNSCOPED_INFO("pos " << pos.t.X() << ',' << pos.t.Y() << ',' << pos.t.Z() << "; " << pos.r.W() << ',' << pos.r.X() << ',' << pos.r.Y() << ',' << pos.r.Z());
    }
    void updateConnectionState(PositionState)
    {}
};

TEST_CASE( "robot moves", "[emulator_laser_mover]" )
{
    QFutureWatcher<IRobotMover::MOVE_RESULT> future_watcher;
    QSignalSpy future_spy(&future_watcher, &QFutureWatcher<IRobotMover::MOVE_RESULT>::finished);
    DebugPositionReceiver pos_receiver;

    EmulatorLaserMover lm(10.0, 180);
    lm.setPositionReceiver(&pos_receiver);

    SECTION("point") {
        QFuture<IRobotMover::MOVE_RESULT> move_result = lm.moveToPosition({{gp_Vec(2,1,1), gp_Quaternion(0.5,0.5,0.5,1)},0,""});

        future_watcher.setFuture(move_result);
        REQUIRE(future_spy.wait(1000));

        CHECK(move_result.result() == IRobotMover::MOVE_OK);
    }

    SECTION("path move") {
        QFuture<IRobotMover::MOVE_RESULT> move_result = lm.moveAlongPath({{{gp_Vec(2,1,1), gp_Quaternion(0.5,0.5,0.5,1)}, 0, ""},
                                                                          {{gp_Vec(1,3,2), gp_Quaternion(-0.5,-0.5,-0.5,1)}, 0, ""}});

        future_watcher.setFuture(move_result);
        REQUIRE(future_spy.wait(1000));

        CHECK(move_result.result() == IRobotMover::MOVE_OK);
    }
}

TEST_CASE( "abort move", "[emulator_laser_mover]" )
{
    QFutureWatcher<IRobotMover::MOVE_RESULT> future_watcher;
    QSignalSpy future_spy(&future_watcher, &QFutureWatcher<IRobotMover::MOVE_RESULT>::finished);
    DebugPositionReceiver pos_receiver;

    // reduce speed
    EmulatorLaserMover lm(5.0, 90);
    lm.setPositionReceiver(&pos_receiver);

    QFuture<IRobotMover::MOVE_RESULT> move_result = lm.moveToPosition({{gp_Vec(2,1,1), gp_Quaternion(0.5,0.5,0.5,1)},0,""});

    future_watcher.setFuture(move_result);

    // wait some time for thread to start
    future_spy.wait(1);

    // not finished yet
    CHECK_FALSE(move_result.isFinished());
    lm.abortMove();
    REQUIRE(future_spy.wait(200));
    CHECK(move_result.result() == IRobotMover::MOVE_USER_ABORT);

    SECTION("resume moving")
    {
        move_result = lm.moveToPosition({{gp_Vec(2,1,1), gp_Quaternion(0.5,0.5,0.5,1)},
                                                    0,
                                                    ""});
        future_watcher.setFuture(move_result);
        REQUIRE(future_spy.wait(1000));

        CHECK(move_result.result() == IRobotMover::MOVE_OK);
    }
}

