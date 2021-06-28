#include "cfanucbotsocket.h"

BotSocket::TSocketError CFanucBotSocket::startSocket()
{
    active_ = true;
    // connect signals so translateModel/rotateModel are called
    QObject::connect(&fanuc_socket_, &FanucSocket::position_received, [&](struct FanucSocket::position pos){
        transformModel(pos.x, pos.y, pos.z, pos.w, pos.p, pos.r);
    });
    QObject::connect(&fanuc_socket_, &FanucSocket::connection_state_changed, [&](bool){
        stateChanged(socketState());
    });
    QObject::connect(&fanuc_socket_, &FanucSocket::attached_changed, [&](bool){
        stateChanged(socketState());
    });
    stateChanged(socketState());
    return BotSocket::ENSE_NO;
}

void CFanucBotSocket::stopSocket()
{
    active_ = false;
    // disconnect all signals
    fanuc_socket_.disconnect();
    stateChanged(socketState());
}

BotSocket::TSocketState CFanucBotSocket::socketState() const
{
    if(fanuc_socket_.connected() && active_)
        return fanuc_socket_.attached() ? BotSocket::ENSS_ATTACHED : BotSocket::ENSS_NOT_ATTACHED;
    else
        return BotSocket::ENSS_FALL;
}
