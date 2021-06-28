#ifndef CBOTSOCKETIMITATOR_H
#define CBOTSOCKETIMITATOR_H

#include <QObject>

#include "cabstractbotsocket.h"

class CBotSocketImitatorPrivate;

class CBotSocketImitator : public QObject, public CAbstractBotSocket
{
    Q_OBJECT

public:
    CBotSocketImitator();
    ~CBotSocketImitator();

    void setMessageInterval(const int interval);

protected:
    BotSocket::TSocketError startSocket() final;
    void stopSocket() final;

    BotSocket::TSocketState socketState() const final;

private slots:
    void slTmTimeout();

private:
    CBotSocketImitatorPrivate * const d_ptr;
};

#endif // CBOTSOCKETIMITATOR_H
