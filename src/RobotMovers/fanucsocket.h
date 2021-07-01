#ifndef FANUCSOCKET_H
#define FANUCSOCKET_H

#include <QObject>
#include <QtNetwork/QTcpSocket>

class FanucSocket : public QObject
{
    Q_OBJECT
public:
    struct position{
        double x,y,z; // mm
        double w,p,r; // radians
    };

    explicit FanucSocket(QObject *parent = nullptr);

    bool connected() const;
    bool attached() const;

signals:
    void position_received(struct position pos);
    void connection_state_changed(bool attached);
    void attached_changed(bool attached);

private slots:
    void on_readyread();
    void on_connected();
    void on_disconnected();
    void on_error(QAbstractSocket::SocketError error);

private:
    void start_connection();

    QTcpSocket socket_;
    position offset_ = {};
    position scale_ = {1,1,1,1,1,1};
    bool attached_ = false;
    bool bigendian_ = false;
};

#endif // FANUCSOCKET_H
