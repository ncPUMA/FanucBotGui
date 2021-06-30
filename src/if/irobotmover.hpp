#ifndef IROBOTMOVER_HPP
#define IROBOTMOVER_HPP

#include <utility>
#include <vector>
#include <QFuture>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>

#include "isceneprovider.hpp"
#include "ipartreferencer.hpp"

class IRobotMover
{
public:
    struct path_point_t {
        position_t pos;
        int timeout_ms = 0;        // stop in this point for ms, optional
        std::string task_filename; // filename with task for externally called programs, optional
    };
    typedef std::vector<path_point_t> path_t;

    enum MOVE_RESULT {
        MOVE_OK = 0,
        MOVE_USER_ABORT,
        MOVE_FAILURE,
        MOVE_UNACHIEVABLE
    };

    virtual ~IRobotMover() = default;

    virtual void setPartReferencer(IPartReferencer *part_referencer) = 0;

    virtual QFuture<MOVE_RESULT> moveToPosition(const path_point_t &position) = 0;
    virtual QFuture<MOVE_RESULT> moveAlongPath(const path_t &path) = 0;

    virtual bool isMoving() const = 0;
    // stops move no matter what
    virtual void abortMove() = 0;
};

#endif // IROBOTMOVER_HPP
