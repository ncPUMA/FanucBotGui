#include "cmodelmover.h"

#include "mainwindow.h"

class CModelMoverPrivate
{
    friend class CModelMover;

    CModelMoverPrivate() :
        gui(nullptr),
        trX(0), trY(0), trZ(0),
        rX(0), rY(0), rZ(0),
        state(BotSocket::ENSS_FALL),
        localIp(0), remoteIp(0),
        localPort(0), remotePort(0)
    { }

    MainWindow *gui;

    BotSocket::TDistance trX, trY, trZ;
    BotSocket::TDegree rX, rY, rZ;
    BotSocket::TSocketState state;
    uint32_t localIp, remoteIp;
    uint16_t localPort, remotePort;
};



CModelMover::CModelMover() :
    d_ptr(new CModelMoverPrivate())
{

}

CModelMover::~CModelMover()
{
    delete d_ptr;
}

BotSocket::TSocketState CModelMover::socketState() const
{
    return d_ptr->state;
}

BotSocket::TDistance CModelMover::getTrX() const
{
    return d_ptr->trX;
}

BotSocket::TDistance CModelMover::getTrY() const
{
    return d_ptr->trY;
}

BotSocket::TDistance CModelMover::getTrZ() const
{
    return d_ptr->trZ;
}

BotSocket::TDegree CModelMover::getRX() const
{
    return d_ptr->rX;
}

BotSocket::TDegree CModelMover::getRY() const
{
    return d_ptr->rY;
}

BotSocket::TDegree CModelMover::getRZ() const
{
    return d_ptr->rZ;
}

void CModelMover::setGui(MainWindow * const gui)
{
    d_ptr->gui = gui;
}

void CModelMover::transformModel(const BotSocket::TDistance trX,
                                 const BotSocket::TDistance trY,
                                 const BotSocket::TDistance trZ,
                                 const BotSocket::TDegree rX,
                                 const BotSocket::TDegree rY,
                                 const BotSocket::TDegree rZ)
{
    d_ptr->trX = trX;
    d_ptr->trY = trY;
    d_ptr->trZ = trZ;
    d_ptr->rX  = rX;
    d_ptr->rY  = rY;
    d_ptr->rZ  = rZ;

    if (d_ptr->gui != nullptr)
        d_ptr->gui->updateMdlTransform();
}

void CModelMover::socketStateChanged(const BotSocket::TSocketState state)
{
    d_ptr->state = state;

    if (d_ptr->gui != nullptr)
        d_ptr->gui->updateBotSocketState();
}
