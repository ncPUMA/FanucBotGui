#ifndef CABSTRACTBOTSOCKETSETTINGS_H
#define CABSTRACTBOTSOCKETSETTINGS_H

#include <cstdint>

class CAbstractBotSocketSettings
{
public:
    CAbstractBotSocketSettings() { }
    virtual ~CAbstractBotSocketSettings() { }

    virtual uint32_t getLocalIpV4() const = 0;
    virtual uint16_t getLocalUdpPort() const = 0;
    virtual uint32_t getRemoteBotIpV4() const = 0;
    virtual uint16_t getRemoteBotUdpPort() const = 0;
};

#endif // CABSTRACTBOTSOCKETSETTINGS_H
