#pragma once

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include "simple_message.h"
#include "fanuc_socket_types.h"

class FanucRelaySocket : public QObject
{
    Q_OBJECT
public:
    explicit FanucRelaySocket(QObject *parent = nullptr);

    bool connected() const;

public slots:
    void move_point(const xyzwpr_data &pos);
    void move_trajectory(const std::vector<xyzwpr_data> &path);
    void move_point(const joint_data &pos);
    void move_trajectory(const std::vector<joint_data> &path);
    void stop();

signals:
    void trajectory_xyzwpr_point_enqueued(const xyzwpr_data &pos, int sequence_number);
    void trajectory_xyzwpr_point_enqueue_fail(const xyzwpr_data &pos, int sequence_number);
    void trajectory_joint_point_enqueued(const joint_data &pos, int sequence_number);
    void trajectory_joint_point_enqueue_fail(const joint_data &pos, int sequence_number);
    void trajectory_enqueue_finished();
    void connection_state_changed(bool connected);

private slots:
    void on_readyread();
    void on_connected();
    void on_disconnected();
    void on_error(QAbstractSocket::SocketError error);

    void move_point(const joint_data &pos, int sequence_number);
    void move_point(const xyzwpr_data &pos, int sequence_number);

private:
    void start_connection();
    bool send_cmd(struct simple_message::joint_traj_pt_t &cmd);
    bool send_cmd(struct simple_message::xyzwpr_traj_pt_t &cmd);

    QTcpSocket socket_;
    bool bigendian_ = false;
    std::vector<joint_data> path_joint_;
    std::vector<xyzwpr_data> path_xyzwpr_;
    simple_message::int_t path_idx_;
    simple_message::int_t prefix1 = 0, prefix2 = 0;
};
