#ifndef SIMPLE_MESSAGE_H
#define SIMPLE_MESSAGE_H

#include <stdint.h>

namespace simple_message
{
    // https://github.com/ros-industrial/rep/blob/master/rep-I0006.rst
    typedef float real_t;
    typedef int32_t int_t;

    enum MSG_TYPE: int_t {
        MSG_TYPE_INVALID   = 0,
        MSG_TYPE_PING               = 1,
        MSG_TYPE_GET_VERSION        = 2,
        MSG_TYPE_JOINT_POSITION     = 10,
        MSG_TYPE_JOINT_TRAJ_PT      = 11,
        MSG_TYPE_JOINT_TRAJ         = 12,
        MSG_TYPE_STATUS             = 13,
        MSG_TYPE_JOINT_TRAJ_PT_FULL = 14,
        MSG_TYPE_JOINT_FEEDBACK     = 15,
        MSG_TYPE_READ_INPUT         = 20,
        MSG_TYPE_READ_OUTPUT        = 21,
        MSG_TYPE_XYZWPR_TRAJ_PT     = 65010
    };

    enum COMM_TYPE: int_t {
        COMM_TYPE_INVALID = 0,
        COMM_TYPE_TOPIC             = 1,
        COMM_TYPE_SERVICE_REQUEST   = 2,
        COMM_TYPE_SERVICE_REPLY     = 3
    };

    enum REPLY_CODE: int_t {
        REPLY_CODE_INVALID = 0,
        REPLY_CODE_SUCCESS = 1,
        REPLY_CODE_FAILURE = 2
    };

    enum SEQUENCE_CODE: int_t {
        START_TRAJECTORY_DOWNLOAD = -1,
        START_TRAJECTORY_STREAMING = -2,
        END_TRAJECTORY = -3,
        STOP_TRAJECTORY = -4
    };

    enum TRI_STATE: int_t {
        TRI_STATE_UNKNOWN = -1,
        TRI_STATE_ON      = 0,
        TRI_STATE_OFF     = 1,
    };

    enum MODE: int_t {
        MODE_UNKNOWN = -1,
        MODE_MANUAL  = 1,
        MODE_AUTO    = 2,
    };

#pragma pack(push, 1)
    struct prefix_t {
        int_t length = 0;
    };

    struct header_t {
        MSG_TYPE   msg_type = MSG_TYPE_INVALID;
        COMM_TYPE  comm_type = COMM_TYPE_INVALID;
        REPLY_CODE reply_code = REPLY_CODE_INVALID;
    };

    struct joint_position_t {
        prefix_t prefix;
        header_t header;
        int_t    sequence = 0;
        real_t   joint_data[10] = {0};
    };

    struct joint_traj_pt_t {
        prefix_t prefix;
        header_t header;
        int_t    sequence = 0;
        real_t   joint_data[10] = {0};
        real_t   velocity = 0;
        real_t   duration = 0;
    };

    struct xyzwpr_t{
        int_t prefix1 = 0, prefix2 = 0;
        real_t xyzwpr[6] = {0};
        int_t config = 0; // T1 | T2 | T3 | flags (need htonl)
    };

    struct xyzwpr_traj_pt_t {
        prefix_t prefix;
        header_t header;
        int_t    sequence = 0;
        real_t   joint_data[10] = {0};
        real_t   velocity = 0;
        real_t   duration = 0;
        xyzwpr_t xyz_data;
    };

    struct status_t {
        prefix_t prefix;
        header_t  header;
        TRI_STATE drives_powered ;
        TRI_STATE e_stopped      ;
        int_t     error_code     ;
        TRI_STATE in_error       ;
        TRI_STATE in_motion      ;
        MODE      mode           ;
        TRI_STATE motion_possible;
    };
#pragma pack(pop)
}

#endif // SIMPLE_MESSAGE_H
