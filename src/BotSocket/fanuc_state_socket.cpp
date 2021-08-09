#include "fanuc_state_socket.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <QTimer>
#include <QSettings>
#include "simple_message.h"

using namespace simple_message;

FanucStateSocket::FanucStateSocket(QObject *parent):
    QObject(parent)
{
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
    qDebug("Connected");
    emit connection_state_changed(true);
}

void FanucStateSocket::on_disconnected()
{
    qDebug("Disconnected, restarting connection");
    QTimer::singleShot(1000, this, &FanucStateSocket::start_connection);
    emit connection_state_changed(false);
}

void FanucStateSocket::on_error(QAbstractSocket::SocketError error)
{
    qDebug() << "Error " << error << " (" << socket_.errorString() << "";
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
            qDebug() << "Service request received, no support";
            header->comm_type = COMM_TYPE_SERVICE_REPLY;
            header->reply_code = REPLY_CODE_FAILURE;
            if(bigendian_)
                bswap(packet);
            socket_.write(packet);
            continue;
        }

        if(header->comm_type != COMM_TYPE_TOPIC && header->comm_type != COMM_TYPE_SERVICE_REPLY)
        {
            qDebug() << "Unknown comm_type "<< header->comm_type;
            continue;
        }

        switch(header->msg_type)
        {
            case MSG_TYPE_JOINT_POSITION: {
                if(prefix->length+sizeof(prefix_t) != sizeof(joint_position_t)) {
                    qDebug() << "Received joint position, but length is not valid";
                    continue;
                }

                struct joint_position_t *msg = reinterpret_cast<struct joint_position_t *>(packet.data());
                joint_data_received(msg->joint_data, msg->sequence);

                break;
            };
            case MSG_TYPE_JOINT_TRAJ_PT: {
                if(prefix->length+sizeof(prefix_t) != sizeof(joint_traj_pt_t)) {
                    qDebug() << "Received joint position, but length is not valid";
                    continue;
                }

                struct joint_traj_pt_t *msg = reinterpret_cast<struct joint_traj_pt_t *>(packet.data());
                joint_data_received(msg->joint_data, msg->sequence);

                break;
            };
            case MSG_TYPE_XYZWPR_TRAJ_PT: {
                if(prefix->length+sizeof(prefix_t) != sizeof(xyzwpr_traj_pt_t)) {
                    qDebug() << "Received xyzwpr_traj_pt_t position, but length is not valid";
                    continue;
                }

                struct xyzwpr_traj_pt_t *msg = reinterpret_cast<struct xyzwpr_traj_pt_t *>(packet.data());
                joint_data_received(msg->joint_data, msg->sequence);
                if(msg->xyz_data.prefix1 != prefix1)
                    qDebug() << "prefix1: received " << msg->xyz_data.prefix1 <<", expected " << prefix1;
//                if(msg->xyz_data.prefix2 != prefix2)
//                    qDebug() << "prefix2: received " << msg->xyz_data.prefix2 <<", expected " << prefix2;
                xyzwpr_data_received(msg->xyz_data.xyzwpr, msg->xyz_data.config, msg->sequence);

                break;
            };
            case MSG_TYPE_STATUS: {
                if(prefix->length+sizeof(prefix_t) != sizeof(status_t)) {
                    qDebug() << "Received joint position, but length is not valid";
                    continue;
                }

                struct status_t *msg = reinterpret_cast<struct status_t *>(packet.data());
//                qDebug() << "STATUS: " << msg->in_motion << msg->drives_powered << msg->motion_possible << msg->mode << msg->e_stopped << msg->in_error << msg->error_code;
                emit status_received(msg->in_motion == TRI_STATE_ON,
                                     msg->drives_powered == TRI_STATE_ON && msg->motion_possible == TRI_STATE_ON,
                                     msg->in_error == TRI_STATE_ON || msg->e_stopped == TRI_STATE_ON);

                break;
            };
            default:
                qDebug() << "Received l=" << prefix->length << " msg=" << header->msg_type << " comm=" << header->comm_type << " reply=" << header->reply_code;
        };
    }
}

void FanucStateSocket::joint_data_received(const simple_message::real_t   joint[10], int)
{
    joint_data pos(6);
    for(int i=0; i<6; i++)
        pos[i] = joint[i] * 180/M_PI;
//    qDebug() << "JOINT POSITION: i=" << sequence << " J1" << pos[0]
//                                                 << " J2" << pos[1]
//                                                 << " J3" << pos[2]
//                                                 << " J4" << pos[3]
//                                                 << " J5" << pos[4]
//                                                 << " J6" << pos[5];
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
//    qDebug() << "XYZWPR POSITION: i=" << sequence << " X" << pos.xyzwpr[0]
//                                                  << " Y" << pos.xyzwpr[1]
//                                                  << " Z" << pos.xyzwpr[2]
//                                                  << " W" << pos.xyzwpr[3]
//                                                  << " P" << pos.xyzwpr[4]
//                                                  << " R" << pos.xyzwpr[5]
//                                                  << " C" << pos.flip << pos.up << pos.top << pos.t1 << pos.t2 << pos.t3;
    emit xyzwpr_position_received(pos);
}

void FanucStateSocket::start_connection()
{
    if(socket_.state() != QAbstractSocket::ConnectingState &&
       socket_.state() != QAbstractSocket::ConnectedState)
    {
        QSettings settings("fanuc.ini", QSettings::IniFormat);

        bigendian_ = settings.value("bigendian", false).toBool();
        QString host = settings.value("server_ip", "127.0.0.1").toString();
        int port = settings.value("server_state_port", 11002).toInt();
        prefix1 = settings.value("prefix1", prefix1).toInt();
        prefix2 = settings.value("prefix2", prefix2).toInt();
        qDebug() << "Connecting to" << host << ":" << port;

        socket_.connectToHost(host, port);
    }
}
