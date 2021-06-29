#include "fanucsocket.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <QTimer>
#include <QSettings>
#include <QtEndian>

// fanuc structures
#pragma pack(push, 1)
    struct xyzwpr{
        uint32_t prefix1, prefix2;
        float x, y, z, w, p, r;
        uint32_t config; // T1 | T2 | T3 | flags (need htonl)
    };

    struct xyzwprext{
        uint32_t prefix1, prefix2;
        float x, y, z, w, p, r;
        uint32_t config;
        float ext1, ext2, ext3;
    };

    struct jointpos{
        uint32_t prefix1, prefix2;
        float j[9];
    };

    struct statepos{
        uint32_t time;
        uint32_t attached;
        struct xyzwpr w;
        struct xyzwpr u;
        struct jointpos j;
    };
#pragma pack(pop)

FanucSocket::FanucSocket(QObject *parent):
    QObject(parent)
{
    assert(sizeof(struct xyzwpr) == 36);
    assert(sizeof(struct xyzwprext) == 48);

    connect(&socket_, &QAbstractSocket::connected, this, &FanucSocket::on_connected);
    connect(&socket_, &QAbstractSocket::disconnected, this, &FanucSocket::on_disconnected);
    connect(&socket_, &QIODevice::readyRead, this, &FanucSocket::on_readyread);

#if QT_VERSION_MAJOR >= 5 && QT_VERSION_MINOR >= 15
    connect(&socket_, &QAbstractSocket::errorOccurred, this, &FanucSocket::on_error);
#else
    connect(&socket_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(on_error(QAbstractSocket::SocketError)));
#endif

    start_connection();
}

bool FanucSocket::connected() const
{
    return socket_.state() == QAbstractSocket::ConnectedState;
}

bool FanucSocket::attached() const
{
    return attached_;
}

void FanucSocket::on_connected()
{
    qDebug("Connected");
    attached_ = false;
    emit connection_state_changed(true);
}

void FanucSocket::on_disconnected()
{
    qDebug("Disconnected, restarting connection");
    QTimer::singleShot(1000, this, &FanucSocket::start_connection);
    emit connection_state_changed(false);
}

void FanucSocket::on_error(QAbstractSocket::SocketError error)
{
    qDebug() << "Error " << error << " (" << socket_.errorString() << "";
    socket_.disconnectFromHost();
    QTimer::singleShot(1000, this, &FanucSocket::start_connection);
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
void FanucSocket::on_readyread()
{
    struct statepos s;
    while(socket_.bytesAvailable() >= (qint64)sizeof(s))
    {
        size_t bytes_read = socket_.read(reinterpret_cast<char*>(&s), sizeof(s));
        if(bytes_read != sizeof(s))
        {
            qDebug("Failed to recv");
        }
        assert(bytes_read == sizeof(s));

        if(bigendian_)
        {
            s.time = bswap(s.time);
            s.attached = bswap(s.attached);

            s.w.x = bswap(s.w.x);
            s.w.y = bswap(s.w.y);
            s.w.z = bswap(s.w.z);
            s.w.w = bswap(s.w.w);
            s.w.p = bswap(s.w.p);
            s.w.r = bswap(s.w.r);
            s.w.config = bswap(s.w.config);

            s.u.x = bswap(s.u.x);
            s.u.y = bswap(s.u.y);
            s.u.z = bswap(s.u.z);
            s.u.w = bswap(s.u.w);
            s.u.p = bswap(s.u.p);
            s.u.r = bswap(s.u.r);
            s.u.config = bswap(s.u.config);
        }
        for(int i=0; i<9; i++)
        {
            if(bigendian_)
                s.j.j[i] = bswap(s.j.j[i]);
            s.j.j[i] = s.j.j[i] * 180/M_PI;
        }

//        qDebug("Pos: t=%f att=%d, u=(%f, %f, %f, %f, %f, %f, %d) w=(%f, %f, %f, %f, %f, %f, %d) j=(%f, %f, %f, %f, %f, %f, %f, %f, %f)\n",
//                     ((double)s.time)/1e6, s.attached,
//                     s.u.x, s.u.y, s.u.z, s.u.w, s.u.p, s.u.r, s.u.config,
//                     s.w.x, s.w.y, s.w.z, s.w.w, s.w.p, s.w.r, s.w.config,
//                     s.j.j[0], s.j.j[1], s.j.j[2], s.j.j[3], s.j.j[4], s.j.j[5], s.j.j[6], s.j.j[7], s.j.j[8]);
    }
    //send last one
    FanucSocket::position pos{scale_.x * s.w.x - offset_.x,
                              scale_.y * s.w.y - offset_.y,
                              scale_.z * s.w.z - offset_.z,
                              scale_.w * s.w.w - offset_.w,
                              scale_.p * s.w.p - offset_.p,
                              scale_.r * s.w.r - offset_.r};
    qDebug("{%f, %d, %f, %f, %f, %f, %f, %f},",
           ((double)s.time)/1e6, s.attached, pos.x, pos.y, pos.z, pos.w, pos.p, pos.r);
    emit position_received(pos);
    bool lock = s.attached != 0;
    if(lock != attached_)
    {
        attached_ = lock;
        emit attached_changed(attached_);
    }
}

void FanucSocket::start_connection()
{
    if(socket_.state() != QAbstractSocket::ConnectingState &&
       socket_.state() != QAbstractSocket::ConnectedState)
    {
        QSettings settings("fanuc.ini", QSettings::IniFormat);

        scale_.x = settings.value("scale_x",1).toFloat();
        scale_.y = settings.value("scale_y",1).toFloat();
        scale_.z = settings.value("scale_z",1).toFloat();
        scale_.w = settings.value("scale_w",1).toFloat();
        scale_.p = settings.value("scale_p",1).toFloat();
        scale_.r = settings.value("scale_r",1).toFloat();

        offset_.x = settings.value("offset_x",0).toFloat();
        offset_.y = settings.value("offset_y",0).toFloat();
        offset_.z = settings.value("offset_z",0).toFloat();
        offset_.w = settings.value("offset_w",0).toFloat();
        offset_.p = settings.value("offset_p",0).toFloat();
        offset_.r = settings.value("offset_r",0).toFloat();

        bigendian_ = settings.value("bigendian", false).toBool();

        QString host = settings.value("server_ip", "127.0.0.1").toString();
        int port = settings.value("server_port", 59002).toInt();
        qDebug() << "Connecting to" << host << ":" << port;

        socket_.connectToHost(host, port);
    }
}
