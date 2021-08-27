#pragma once

#include <QObject>
#include <QTimer>
#include <QtNetwork/QTcpSocket>
#include "fanuc_socket_types.h"
#include "simple_message.h"

class FanucStateSocket : public QObject
{
    Q_OBJECT
public:
    explicit FanucStateSocket(QObject *parent = nullptr);

    bool connected() const;

signals:
    void joint_position_received(const joint_data &pos);
    void xyzwpr_position_received(const xyzwpr_data &pos);
    void connection_state_changed(bool connected);
    void status_received(bool moving, bool ready_to_move, bool error);

private slots:
    void on_readyread();
    void on_connected();
    void on_disconnected();
    void on_error(QAbstractSocket::SocketError error);

    void joint_data_received(const simple_message::real_t   joint_data[10], int sequence);
    void xyzwpr_data_received(const simple_message::real_t   xyzwpr_data[6], int config, int sequence);

    void watchdog();

private:
    void start_connection();

    QTcpSocket socket_;
    bool bigendian_ = false;
    simple_message::int_t prefix1 = 0, prefix2 = 0;
    bool watchdog_ = true;
    QTimer watchdog_timer_;
};
