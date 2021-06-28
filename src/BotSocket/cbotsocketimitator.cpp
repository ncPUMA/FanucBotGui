#include "cbotsocketimitator.h"

#include <QTimer>

#include "fanuc_imitation_data.h"

static const int DEFAULT_TM_INTERVAL = 20;

class CBotSocketImitatorPrivate
{
    friend class CBotSocketImitator;

    CBotSocketImitatorPrivate() :
        tx(0), ty(0), tz(0),
        rx(0), ry(0), rz(0)
    { }

    QTimer tm;
    double tx, ty, tz;
    double rx, ry, rz;

    size_t imitation_data_idx = 0;
    bool attached = false;
};

CBotSocketImitator::CBotSocketImitator() :
    CAbstractBotSocket(),
    d_ptr(new CBotSocketImitatorPrivate())
{
    d_ptr->tm.setInterval(DEFAULT_TM_INTERVAL);
    connect(&d_ptr->tm, SIGNAL(timeout()), SLOT(slTmTimeout()));
}

CBotSocketImitator::~CBotSocketImitator()
{
    delete d_ptr;
}

void CBotSocketImitator::setMessageInterval(const int interval)
{
    d_ptr->tm.setInterval(interval);
}

BotSocket::TSocketError CBotSocketImitator::startSocket()
{
    d_ptr->tm.start();
    return BotSocket::ENSE_NO;
}

void CBotSocketImitator::stopSocket()
{
    d_ptr->tm.stop();
}

BotSocket::TSocketState CBotSocketImitator::socketState() const
{
    return d_ptr->attached ? BotSocket::ENSS_ATTACHED : BotSocket::ENSS_NOT_ATTACHED;
}

void CBotSocketImitator::slTmTimeout()
{
    fanuc_imitation_pos pos = fanuc_imitation_data[d_ptr->imitation_data_idx];
    d_ptr->imitation_data_idx = (d_ptr->imitation_data_idx + 1) % fanuc_imitation_data_size;
    if (d_ptr->attached != pos.attached)
    {
        d_ptr->attached = pos.attached;
        stateChanged(d_ptr->attached ? BotSocket::ENSS_ATTACHED : BotSocket::ENSS_NOT_ATTACHED);
    }
    transformModel(pos.x, pos.y, pos.z,
                   pos.w, pos.p, pos.r);


//    d_ptr->tx += .5;
//    if (d_ptr->tx >= 50.)
//        d_ptr->tx = 0;
//    d_ptr->rx += .5;
//    if (d_ptr->rx >= 90.)
//        d_ptr->rx = 0;

//    transformModel(d_ptr->tx, d_ptr->ty, d_ptr->tz,
//                   d_ptr->rx, d_ptr->ry, d_ptr->rz);
}
