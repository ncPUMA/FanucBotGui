#ifndef CFANUCBOTSOCKET_H
#define CFANUCBOTSOCKET_H

#include "cabstractbotsocket.h"
#include "fanuc_state_socket.h"
#include "fanuc_relay_socket.h"

class CFanucBotSocket:
        public QObject,
        public CAbstractBotSocket
{
    Q_OBJECT
public:
    CFanucBotSocket();

    BotSocket::EN_CalibResult execCalibration(const std::vector <GUI_TYPES::SCalibPoint> &points);
    void prepare(const std::vector <GUI_TYPES::STaskPoint> &points);
    void startTasks(const std::vector <GUI_TYPES::SPathPoint> &pathPoints,
                    const std::vector <GUI_TYPES::STaskPoint> &taskPoints);
    void stopTasks();
    void shapeTransformChanged(const BotSocket::EN_ShapeType shType);

private:

    FanucStateSocket fanuc_state_;
    FanucRelaySocket fanuc_relay_;

    void updatePosition(const xyzwpr_data &pos);
    void updateConnectionState();
};

#endif // CFANUCBOTSOCKET_H
