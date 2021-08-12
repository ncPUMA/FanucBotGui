#include "fanuc_state_socket.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <QTimer>
#include <QSettings>
#include "simple_message.h"
#include "log/loguru.hpp"

using namespace simple_message;

FanucStateSocket::FanucStateSocket(QObject *parent):
    QObject(parent)
{
    VLOG_CALL;
    connect(&socket_, &QAbstractSocket::connected, this, &FanucStateSocket::on_connected);
    connect(&socket_, &QAbstractSocket::disconnected, this, &FanucStateSocket::on_disconnected);
    connect(&socket_, &QIODevice::readyRead, this, &FanucStateSocket::on_readyread);

#if QT_VERSION_MAJOR >= 5 && QT_VERSION_MINOR >= 15
    connect(&socket_, &QAbstractSocket::errorOccurred, this, &FanucStateSocket::on_error);
#else
    connect(&socket_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(on_error(QAbstractSocket::SocketError)));
#endif

    start_connection();
}

bool FanucStateSocket::connected() const
{
    return socket_.state() == QAbstractSocket::ConnectedState;
}

void FanucStateSocket::on_connected()
{
    VLOG_CALL;
    emit connection_state_changed(true);
}

void FanucStateSocket::on_disconnected()
{
    VLOG_CALL;
    QTimer::singleShot(1000, this, &FanucStateSocket::start_connection);
    emit connection_state_changed(false);
}

void FanucStateSocket::on_error(QAbstractSocket::SocketError error)
{
    VLOG_CALL;
    LOG_F(ERROR, "%d (%s)", error, socket_.errorString().toLocal8Bit().data());
    socket_.disconnectFromHost();
    QTimer::singleShot(1000, this, &FanucStateSocket::start_connection);
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

void FanucStateSocket::on_readyread()
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
            LOG_F(ERROR, "Service request received, no support");
            header->comm_type = COMM_TYPE_SERVICE_REPLY;
            header->reply_code = REPLY_CODE_FAILURE;
            if(bigendian_)
                bswap(packet);
            socket_.write(packet);
            continue;
        }

        if(header->comm_type != COMM_TYPE_TOPIC && header->comm_type != COMM_TYPE_SERVICE_REPLY)
        {
            LOG_F(ERROR, "Unknown comm_type %d", header->comm_type);
            continue;
        }

        switch(header->msg_type)
        {
            case MSG_TYPE_JOINT_POSITION: {
                if(prefix->length+sizeof(prefix_t) != sizeof(joint_position_t)) {
                    LOG_F(ERROR, "Received joint position, but length is not valid");
                    continue;
                }

                struct joint_position_t *msg = reinterpret_cast<struct joint_position_t *>(packet.data());
                joint_data_received(msg->joint_data, msg->sequence);

                break;
            };
            case MSG_TYPE_JOINT_TRAJ_PT: {
                if(prefix->length+sizeof(prefix_t) != sizeof(joint_traj_pt_t)) {
                    LOG_F(ERROR, "Received joint traj pt, but length is not valid");
                    continue;
                }

                struct joint_traj_pt_t *msg = reinterpret_cast<struct joint_traj_pt_t *>(packet.data());
                joint_data_received(msg->joint_data, msg->sequence);

                break;
            };
            case MSG_TYPE_XYZWPR_TRAJ_PT: {
                if(prefix->length+sizeof(prefix_t) != sizeof(xyzwpr_traj_pt_t)) {
                    LOG_F(ERROR, "Received xyzwpr_traj_pt_t, but length is not valid");
                    continue;
                }

                struct xyzwpr_traj_pt_t *msg = reinterpret_cast<struct xyzwpr_traj_pt_t *>(packet.data());
                joint_data_received(msg->joint_data, msg->sequence);
                if(msg->xyz_data.prefix1 != prefix1)
                    LOG_F(5, "prefix1: received %d, expected: %d", msg->xyz_data.prefix1, prefix1);
                if(msg->xyz_data.prefix2 != prefix2)
                    LOG_F(5, "prefix2: received %d, expected: %d", msg->xyz_data.prefix2, prefix2);
                xyzwpr_data_received(msg->xyz_data.xyzwpr, msg->xyz_data.config, msg->sequence);

                break;
            };
            case MSG_TYPE_STATUS: {
                if(prefix->length+sizeof(prefix_t) != sizeof(status_t)) {
                    LOG_F(ERROR, "Received status, but length is not valid");
                    continue;
                }

                struct status_t *msg = reinterpret_cast<struct status_t *>(packet.data());
                LOG_F(INFO, "STATUS: in_motion=%d drives_powered=%d motion_possible=%d mode=%d e_stopped=%d in_error=%d error_code=%d",
                                     msg->in_motion, msg->drives_powered, msg->motion_possible, msg->mode, msg->e_stopped, msg->in_error, msg->error_code);
                emit status_received(msg->in_motion == TRI_STATE_ON,
                                     msg->drives_powered == TRI_STATE_ON && msg->motion_possible == TRI_STATE_ON,
                                     msg->in_error == TRI_STATE_ON || msg->e_stopped == TRI_STATE_ON);

                break;
            };
            default:
                LOG_F(INFO, "Received l=%d msg=%d comm=%d reply=%d", prefix->length, header->msg_type, header->comm_type, header->reply_code);
        };
    }
}

void FanucStateSocket::joint_data_received(const simple_message::real_t   joint[10], int)
{
    joint_data pos(6);
    for(int i=0; i<6; i++)
        pos[i] = joint[i] * 180/M_PI;
    LOG_F(4, "JOINT POSITION: J1=%f J2=%f J3=%f J4=%f J5=%f J6=%f", pos[0], pos[1], pos[2],
                                                                    pos[3], pos[4], pos[5]);
    emit joint_position_received(pos);
}
void FanucStateSocket::xyzwpr_data_received(const simple_message::real_t   xyzwpr[6], int config, int)
{
    xyzwpr_data pos;
    pos.xyzwpr.resize(6);
    for(int i=0; i<6; i++)
        pos.xyzwpr[i] = xyzwpr[i];
    if(bigendian_) // config is not byte-swapped
        config = bswap(config);
    fanuc_config_parse(config, pos);
    LOG_F(4, "XYZWPR POSITION: X=%f Y=%f Z=%f W=%f P=%f R=%f, C=(%d,%d,%d,%d,%d,%d)",
                                                               pos.xyzwpr[0], pos.xyzwpr[1], pos.xyzwpr[2],
                                                               pos.xyzwpr[3], pos.xyzwpr[4], pos.xyzwpr[5],
                                                               pos.flip, pos.up, pos.top, pos.t1, pos.t2, pos.t3);
    emit xyzwpr_position_received(pos);
}

void FanucStateSocket::start_connection()
{
    VLOG_CALL;

    if(socket_.state() != QAbstractSocket::ConnectingState &&
       socket_.state() != QAbstractSocket::ConnectedState)
    {
        QSettings settings("fanuc.ini", QSettings::IniFormat);

        bigendian_ = settings.value("bigendian", false).toBool();
        QString host = settings.value("server_ip", "127.0.0.1").toString();
        int port = settings.value("server_state_port", 11002).toInt();
        prefix1 = settings.value("prefix1", prefix1).toInt();
        prefix2 = settings.value("prefix2", prefix2).toInt();
        LOG_F(INFO, "Connecting to %s:%d", host.toLocal8Bit().data(), port);
        LOG_F(INFO, "Bigendian: %d, prefix1: %d, prefix2: %d", bigendian_, prefix1, prefix2);

        socket_.connectToHost(host, port);
    }
}
