#ifndef CABSTRACTUI_H
#define CABSTRACTUI_H

#include "bot_socket_types.h"

class CAbstractBotSocket;

class CAbstractUi
{
    friend class CAbstractBotSocket;

public:
    CAbstractUi() { }
    virtual ~CAbstractUi() { }

protected:
    virtual void transformModel(const BotSocket::TDistance trX,
                                const BotSocket::TDistance trY,
                                const BotSocket::TDistance trZ,
                                const BotSocket::TDegree rX,
                                const BotSocket::TDegree rY,
                                const BotSocket::TDegree rZ) = 0;

    virtual void socketStateChanged(const BotSocket::TSocketState state) = 0;
};

#endif // CABSTRACTUI_H
