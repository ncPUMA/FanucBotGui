#include "fanuc_relay_socket.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <QTimer>
#include <QSettings>
#include "log/loguru.hpp"

using namespace simple_message;

FanucRelaySocket::FanucRelaySocket(QObject *parent):
    QObject(parent)
{
    VLOG_CALL;

    connect(&socket_, &QAbstractSocket::connected, this, &FanucRelaySocket::on_connected);
    connect(&socket_, &QAbstractSocket::disconnected, this, &FanucRelaySocket::on_disconnected);
    connect(&socket_, &QIODevice::readyRead, this, &FanucRelaySocket::on_readyread);

#if QT_VERSION_MAJOR >= 5 && QT_VERSION_MINOR >= 15
    connect(&socket_, &QAbstractSocket::errorOccurred, this, &FanucRelaySocket::on_error);
#else
    connect(&socket_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(on_error(QAbstractSocket::SocketError)));
#endif

    start_connection();
}

bool FanucRelaySocket::connected() const
{
    return socket_.state() == QAbstractSocket::ConnectedState;
}

void FanucRelaySocket::on_connected()
{
    VLOG_CALL;
    emit connection_state_changed(true);
}

void FanucRelaySocket::on_disconnected()
{
    VLOG_CALL;
    QTimer::singleShot(1000, this, &FanucRelaySocket::start_connection);
    emit connection_state_changed(false);
}

void FanucRelaySocket::on_error(QAbstractSocket::SocketError error)
{
    VLOG_CALL;
    LOG_F(ERROR, "%d (%s)", error, socket_.errorString().toLocal8Bit().data());
    socket_.disconnectFromHost();
    QTimer::singleShot(1000, this, &FanucRelaySocket::start_connection);
    emit connection_state_changed(false);
}

// works with both floats and ints
template <typename T>
T bswap(T val) {
    T retVal;
    char *pVal = (char*) &val;
    char *pRetVal = (char*)&retVal;
    int size = sizeof(T);
    for(int i=0; i<size; i++) {
        pRetVal[size-1-i] = pVal[i];
    }

    return retVal;
}

static void bswap(QByteArray &array)
{
    for(int i=0; i<(array.size()/4)*4; i+=4)
    {
        char tmp0 = array[i+0], tmp1 = array[i+1];
        array[i+0] = array[i+3];
        array[i+1] = array[i+2];
        array[i+2] = tmp1;
        array[i+3] = tmp0;
    }
}

static bool get_prefix_header(QByteArray &packet, struct prefix_t **prefix, struct header_t **header)
{
    if(static_cast<size_t>(packet.size()) < sizeof(prefix_t) + sizeof(header_t))
        return false;

    char *data = packet.data();
    *prefix = reinterpret_cast<prefix_t*>(data);
    *header = reinterpret_cast<header_t*>(data + sizeof(prefix_t));

    return true;
}

void FanucRelaySocket::on_readyread()
{
    VLOG_CALL;

    struct prefix_t *prefix = nullptr;
    struct header_t *header = nullptr;
    while(socket_.bytesAvailable() != 0)
    {
        QByteArray packet = socket_.readAll();

        if(!get_prefix_header(packet, &prefix, &header))
            continue;

        if(bigendian_)
            bswap(packet);

        if(header->comm_type == COMM_TYPE_SERVICE_REQUEST)
        {
            LOG_F(ERROR, "Service request received, no support");
            header->comm_type = COMM_TYPE_SERVICE_REPLY;
            header->reply_code = REPLY_CODE_FAILURE;
            if(bigendian_)
                bswap(packet);
            socket_.write(packet);
            continue;
        }

        if(header->comm_type == COMM_TYPE_TOPIC)
        {
            LOG_F(WARNING, "Topic received, ignoring");
            continue;
        }

        if(header->comm_type != COMM_TYPE_SERVICE_REPLY)
        {
            LOG_F(ERROR, "Unknown comm_type %d", header->comm_type);
            continue;
        }

        simple_message::int_t sequence_id = 0;
        switch(header->msg_type)
        {
            case MSG_TYPE_JOINT_POSITION: {
                if(prefix->length+sizeof(prefix_t) != sizeof(joint_position_t)) {
                    LOG_F(ERROR, "Received joint position, but length is not valid");
                    continue;
                }
                struct joint_position_t *msg = reinterpret_cast<struct joint_position_t *>(packet.data());
                sequence_id = msg->sequence;

                break;
            };
            case MSG_TYPE_JOINT_TRAJ_PT: {
                if(prefix->length+sizeof(prefix_t) != sizeof(joint_traj_pt_t)) {
                    LOG_F(ERROR, "Received joint traj pt, but length is not valid");
                    continue;
                }
                struct joint_traj_pt_t *msg = reinterpret_cast<struct joint_traj_pt_t *>(packet.data());
                sequence_id = msg->sequence;

                break;
            };
            case MSG_TYPE_XYZWPR_TRAJ_PT: {
                if(prefix->length+sizeof(prefix_t) != sizeof(xyzwpr_traj_pt_t)) {
                    LOG_F(ERROR, "Received xyzwpr_traj_pt_t, but length is not valid");
                    continue;
                }
                struct xyzwpr_traj_pt_t *msg = reinterpret_cast<struct xyzwpr_traj_pt_t *>(packet.data());
                sequence_id = msg->sequence;

                break;
            };
            default:
            {
                LOG_F(INFO, "Received l=%d msg=%d comm=%d reply=%d", prefix->length, header->msg_type, header->comm_type, header->reply_code);
                continue;
            }
        };

        if(header->reply_code == REPLY_CODE_SUCCESS)
        {
            if(sequence_id < 0)
            {
                if(sequence_id == STOP_TRAJECTORY && sequence_id == path_idx_)
                {
                    LOG_F(INFO, "Stop trajectory completed");
                }
                else
                {
                    LOG_F(WARNING, "Unexpected sequence_id %d", sequence_id);
                }
                continue;
            }

            if(sequence_id != path_idx_)
            {
                LOG_F(WARNING, "Unexpected sequence_id %d, expected %d", sequence_id, path_idx_);
                continue;
            }

            LOG_F(INFO, "Trajectory point enqueued %d", path_idx_);

            size_t path_idx_u = path_idx_;
            if(path_idx_u < path_joint_.size())
            {
                emit trajectory_joint_point_enqueued(path_joint_[path_idx_], path_idx_);

                path_idx_++;
                path_idx_u++;
                if(path_idx_u < path_joint_.size())
                {
                    move_point(path_joint_[path_idx_], path_idx_);
                }
                else
                {
                    emit trajectory_enqueue_finished();
                }
            }
            else if(path_idx_u < path_xyzwpr_.size())
            {
                emit trajectory_xyzwpr_point_enqueued(path_xyzwpr_[path_idx_], path_idx_);

                path_idx_++;
                path_idx_u++;
                if(path_idx_u < path_xyzwpr_.size())
                {
                    move_point(path_xyzwpr_[path_idx_], path_idx_);
                }
                else
                {
                    emit trajectory_enqueue_finished();
                }
            }
            else
            {
                LOG_F(ERROR, "Failure: unknown path_idx %d", path_idx_);
            }
        }
        else if(header->reply_code == REPLY_CODE_FAILURE)
        {
            if(sequence_id < 0 && sequence_id == path_idx_)
            {
                if(sequence_id == STOP_TRAJECTORY)
                {
                    LOG_F(ERROR, "Stop trajectory failed");
                }
                else
                {
                    LOG_F(ERROR, "Unexpected sequence_id %d", sequence_id);
                }
                continue;
            }

            LOG_F(ERROR, "Trajectory point enqueue fail, stopping %d", path_idx_);

            size_t path_idx_u = path_idx_;
            if(path_idx_u < path_joint_.size())
            {
                emit trajectory_joint_point_enqueue_fail(path_joint_[path_idx_], path_idx_);
            }
            else if(path_idx_u < path_xyzwpr_.size())
            {
                emit trajectory_xyzwpr_point_enqueue_fail(path_xyzwpr_[path_idx_], path_idx_);
            }
            else
            {
                LOG_F(ERROR, "Failure: unknown path_idx %d", path_idx_);
            }

            stop();
        }
        else
        {
            LOG_F(WARNING, "Unexpected reply_code %d", header->reply_code);
            continue;
        }
    }
}

void FanucRelaySocket::start_connection()
{
    VLOG_CALL;
    if(socket_.state() != QAbstractSocket::ConnectingState &&
       socket_.state() != QAbstractSocket::ConnectedState)
    {
        QSettings settings("fanuc.ini", QSettings::IniFormat);

        bigendian_ = settings.value("bigendian", false).toBool();
        QString host = settings.value("server_ip", "127.0.0.1").toString();
        int port = settings.value("server_relay_port", 11000).toInt();
        prefix1_ = settings.value("prefix1", prefix1_).toInt();
        prefix2_ = settings.value("prefix2", prefix2_).toInt();
        LOG_F(INFO, "Connecting to %s:%d", host.toLocal8Bit().data(), port);
        LOG_F(INFO, "Bigendian: %d, prefix1: %d, prefix2: %d", bigendian_, prefix1_, prefix2_);

        socket_.connectToHost(host, port);
    }
}

bool FanucRelaySocket::send_cmd(struct simple_message::joint_traj_pt_t &cmd)
{
    if(socket_.state() != QAbstractSocket::ConnectedState)
    {
        return false;
    }
    path_idx_ = cmd.sequence;
    cmd.header.msg_type = MSG_TYPE_JOINT_TRAJ_PT;
    cmd.header.comm_type = COMM_TYPE_SERVICE_REQUEST;
    cmd.prefix.length = sizeof(joint_traj_pt_t) - sizeof(prefix_t);

    QByteArray packet(reinterpret_cast<const char*>(&cmd), sizeof(cmd));
    if(bigendian_)
        bswap(packet);

    socket_.write(packet);

    return true;
}

bool FanucRelaySocket::send_cmd(struct simple_message::xyzwpr_traj_pt_t &cmd)
{
    if(socket_.state() != QAbstractSocket::ConnectedState)
    {
        return false;
    }
    path_idx_ = cmd.sequence;
    cmd.header.msg_type = MSG_TYPE_XYZWPR_TRAJ_PT;
    cmd.header.comm_type = COMM_TYPE_SERVICE_REQUEST;
    cmd.prefix.length = sizeof(xyzwpr_traj_pt_t) - sizeof(prefix_t);

    QByteArray packet(reinterpret_cast<const char*>(&cmd), sizeof(cmd));
    if(bigendian_)
        bswap(packet);

    socket_.write(packet);
    return true;
}

void FanucRelaySocket::stop()
{
    VLOG_CALL;
    path_xyzwpr_.clear();
    path_joint_.clear();
    struct joint_traj_pt_t cmd;
    cmd.sequence = STOP_TRAJECTORY;
    send_cmd(cmd);
}

void FanucRelaySocket::move_point(const joint_data &pos)
{
    path_joint_ = {pos};
    move_trajectory(path_joint_);
}

void FanucRelaySocket::move_trajectory(const std::vector<joint_data> &path)
{
    VLOG_CALL;
    path_xyzwpr_.clear();
    path_joint_ = path;
    move_point(path_joint_[0], 0);
}

void FanucRelaySocket::move_point(const joint_data &pos, int sequence_number)
{
    VLOG_CALL;
    struct joint_traj_pt_t cmd;
    cmd.sequence = sequence_number;
    for(int i=0; i<6; i++)
        cmd.joint_data[i] = pos[i] * M_PI/180;
    LOG_F(INFO, "JOINT TRAJ PT: i=%d J1=%f J2=%f J3=%f J4=%f J5=%f J6=%f", cmd.sequence, pos[0], pos[1], pos[2],
                                                                                         pos[3], pos[4], pos[5]);
    if(!send_cmd(cmd))
        emit trajectory_joint_point_enqueue_fail(pos, sequence_number);
}

void FanucRelaySocket::move_point(const xyzwpr_data &pos)
{
    path_xyzwpr_ = {pos};
    move_trajectory(path_xyzwpr_);
}

void FanucRelaySocket::move_trajectory(const std::vector<xyzwpr_data> &path)
{
    VLOG_CALL;
    path_joint_.clear();
    path_xyzwpr_ = path;
    move_point(path_xyzwpr_[0], 0);
}

void FanucRelaySocket::move_point(const xyzwpr_data &pos, int sequence_number)
{
    struct xyzwpr_traj_pt_t cmd;
    cmd.sequence = sequence_number;
    cmd.xyz_data.config = fanuc_config_make(pos);
    if(bigendian_)
    {
        // config is not byte-swapped
        cmd.xyz_data.config = bswap(cmd.xyz_data.config);
    }
    for(int i=0; i<6; i++)
        cmd.xyz_data.xyzwpr[i] = pos.xyzwpr[i];
    cmd.xyz_data.prefix1 = prefix1_;
    cmd.xyz_data.prefix2 = prefix2_;
    LOG_F(INFO, "XYZWPR TRAJ PT: i=%d X=%f Y=%f Z=%f W=%f P=%f R=%f", cmd.sequence, pos.xyzwpr[0], pos.xyzwpr[1], pos.xyzwpr[2],
                                                                                    pos.xyzwpr[3], pos.xyzwpr[4], pos.xyzwpr[5]);
    if(!send_cmd(cmd))
        emit trajectory_xyzwpr_point_enqueue_fail(pos, sequence_number);
}
