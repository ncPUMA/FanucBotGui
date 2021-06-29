#include "cabstractbotsocket.h"

#include "cabstractui.h"

static class CEmptyUi : public CAbstractUi
{
    friend class CAbstractBotSocket;

protected:
    void socketStateChanged(const BotSocket::TBotState) final { }
    void laserHeadPositionChanged(const BotSocket::SBotPosition &) final { }
    void gripPositionChanged(const BotSocket::SBotPosition &) final { }

} emptyUi;



CAbstractBotSocket::~CAbstractBotSocket()
{

}

CAbstractBotSocket::CAbstractBotSocket() :
    ui(&emptyUi)
{

}

void CAbstractBotSocket::socketStateChanged(const BotSocket::TBotState state)
{
    ui->socketStateChanged(state);
}

void CAbstractBotSocket::laserHeadPositionChanged(const BotSocket::SBotPosition &pos)
{
    ui->laserHeadPositionChanged(pos);
}

void CAbstractBotSocket::gripPositionChanged(const BotSocket::SBotPosition &pos)
{
    ui->gripPositionChanged(pos);
}
