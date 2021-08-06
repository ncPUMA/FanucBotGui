#include "fanuc_relay_socket.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <QTimer>
#include <QSettings>

using namespace simple_message;

FanucRelaySocket::FanucRelaySocket(QObject *parent):
    QObject(parent)
{
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
    qDebug("Connected");
    emit connection_state_changed(true);
}

void FanucRelaySocket::on_disconnected()
{
    qDebug("Disconnected, restarting connection");
    QTimer::singleShot(1000, this, &FanucRelaySocket::start_connection);
    emit connection_state_changed(false);
}

void FanucRelaySocket::on_error(QAbstractSocket::SocketError error)
{
    qDebug() << "Error " << error << " (" << socket_.errorString() << "";
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
    if(packet.size() < sizeof(prefix_t) + sizeof(header_t))
        return false;

    char *data = packet.data();
    *prefix = reinterpret_cast<prefix_t*>(data);
    *header = reinterpret_cast<header_t*>(data + sizeof(prefix_t));

    return true;
}

void FanucRelaySocket::on_readyread()
{
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
            qDebug() << "Service request received, no support";
            header->comm_type = COMM_TYPE_SERVICE_REPLY;
            header->reply_code = REPLY_CODE_FAILURE;
            if(bigendian_)
                bswap(packet);
            socket_.write(packet);
            continue;
        }

        if(header->comm_type == COMM_TYPE_TOPIC)
        {
            qDebug() << "Topic received, ignoring";
            continue;
        }

        if(header->comm_type != COMM_TYPE_SERVICE_REPLY)
        {
            qDebug() << "Unknown comm_type "<< header->comm_type;
            continue;
        }

        simple_message::int_t sequence_id = 0;
        switch(header->msg_type)
        {
            case MSG_TYPE_JOINT_POSITION: {
                if(prefix->length+sizeof(prefix_t) != sizeof(joint_position_t)) {
                    qDebug() << "Received joint position, but length is not valid";
                    continue;
                }
                struct joint_position_t *msg = reinterpret_cast<struct joint_position_t *>(packet.data());
                sequence_id = msg->sequence;

                break;
            };
            case MSG_TYPE_JOINT_TRAJ_PT: {
                if(prefix->length+sizeof(prefix_t) != sizeof(joint_traj_pt_t)) {
                    qDebug() << "Received joint position, but length is not valid";
                    continue;
                }
                struct joint_traj_pt_t *msg = reinterpret_cast<struct joint_traj_pt_t *>(packet.data());
                sequence_id = msg->sequence;

                break;
            };
            case MSG_TYPE_XYZWPR_TRAJ_PT: {
                if(prefix->length+sizeof(prefix_t) != sizeof(xyzwpr_traj_pt_t)) {
                    qDebug() << "Received joint position, but length is not valid";
                    continue;
                }
                struct xyzwpr_traj_pt_t *msg = reinterpret_cast<struct xyzwpr_traj_pt_t *>(packet.data());
                sequence_id = msg->sequence;

                break;
            };
            default:
            {
                qDebug() << "Received l=" << prefix->length << " msg=" << header->msg_type << " comm=" << header->comm_type << " reply=" << header->reply_code;
                continue;
            }
        };

        if(header->reply_code == REPLY_CODE_SUCCESS)
        {
            if(sequence_id != path_idx_)
            {
                qDebug() << "Unexpected sequence_id " << sequence_id << " expected " << path_idx_;
                continue;
            }

            qDebug() << "Trajectory point enqueued "<< path_idx_;

            if(path_idx_ < path_joint_.size())
            {
                emit trajectory_joint_point_enqueued(path_joint_[path_idx_], path_idx_);

                path_idx_++;
                if(path_idx_ < path_joint_.size())
                {
                    move_point(path_joint_[path_idx_], path_idx_);
                }
                else
                {
                    emit trajectory_enqueue_finished();
                }
            }
            else if(path_idx_ < path_xyzwpr_.size())
            {
                emit trajectory_xyzwpr_point_enqueued(path_xyzwpr_[path_idx_], path_idx_);

                path_idx_++;
                if(path_idx_ < path_xyzwpr_.size())
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
                qDebug() << "Failure: unknown path_idx";
            }
        }
        else if(header->reply_code == REPLY_CODE_FAILURE)
        {
            qDebug() << "Trajectory point enqueue fail, stopping "<< path_idx_;

            if(path_idx_ < path_joint_.size())
            {
                emit trajectory_joint_point_enqueue_fail(path_joint_[path_idx_], path_idx_);
            }
            else if(path_idx_ < path_xyzwpr_.size())
            {
                emit trajectory_xyzwpr_point_enqueue_fail(path_xyzwpr_[path_idx_], path_idx_);
            }
            else
            {
                qDebug() << "Failure: unknown path_idx";
            }

            stop();
        }
        else
        {
            qDebug() << "Unexpected reply_code "<< header->reply_code;
            continue;
        }
    }
}

void FanucRelaySocket::start_connection()
{
    if(socket_.state() != QAbstractSocket::ConnectingState &&
       socket_.state() != QAbstractSocket::ConnectedState)
    {
        QSettings settings("fanuc.ini", QSettings::IniFormat);

        bigendian_ = settings.value("bigendian", false).toBool();
        QString host = settings.value("server_ip", "192.168.1.230").toString();
        int port = settings.value("server_relay_port", 11000).toInt();
        prefix1 = settings.value("prefix1", prefix1).toInt();
        prefix2 = settings.value("prefix2", prefix2).toInt();
        qDebug() << "Connecting to" << host << ":" << port;

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
    path_xyzwpr_.clear();
    path_joint_ = path;
    move_point(path_joint_[0], 0);
}

void FanucRelaySocket::move_point(const joint_data &pos, int sequence_number)
{
    struct joint_traj_pt_t cmd;
    cmd.sequence = sequence_number;
    for(int i=0; i<6; i++)
        cmd.joint_data[i] = pos[i] * M_PI/180;
    qDebug() << "JOINT TRAJ PT: i=" << cmd.sequence << " J1" << pos[0]
                                                   << " J2" << pos[1]
                                                   << " J3" << pos[2]
                                                   << " J4" << pos[3]
                                                   << " J5" << pos[4]
                                                   << " J6" << pos[5];
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
    cmd.xyz_data.prefix1 = prefix1;
    cmd.xyz_data.prefix2 = prefix2;
    qDebug() << "JOINT TRAJ PT: i=" << cmd.sequence << " X" << pos.xyzwpr[0]
                                                   << " Y" << pos.xyzwpr[1]
                                                   << " Z" << pos.xyzwpr[2]
                                                   << " W" << pos.xyzwpr[3]
                                                   << " P" << pos.xyzwpr[4]
                                                   << " R" << pos.xyzwpr[5];
    if(!send_cmd(cmd))
        emit trajectory_xyzwpr_point_enqueue_fail(pos, sequence_number);
}
