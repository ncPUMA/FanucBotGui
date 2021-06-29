#ifndef BOT_SOCKET_TYPES_H
#define BOT_SOCKET_TYPES_H

namespace BotSocket
{

typedef double TDistance;
typedef double TDegree;

enum EN_SocketErrors
{
    ENSE_NO = 0
};
typedef int TSocketError;

enum EN_SocketState
{
    ENSS_FALL,
    ENSS_NOT_ATTACHED,
    ENSS_ATTACHED
};
typedef int TSocketState;

}

#endif // BOT_SOCKET_TYPES_H
