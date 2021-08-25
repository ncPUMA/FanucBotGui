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
    void startTasks(const std::vector <GUI_TYPES::SPathPoint> &,
                    const std::vector <GUI_TYPES::STaskPoint> &taskPoints);
    void stopTasks();
    void shapeTransformChanged(const BotSocket::EN_ShapeType shType);

private:

    FanucStateSocket fanuc_state_;
    FanucRelaySocket fanuc_relay_;

    gp_Trsf world2user_, user2world_;

    void updatePosition(const xyzwpr_data &pos);
    void updateConnectionState();

private:
    void completePath(const BotSocket::EN_WorkResult result);
    void calibFinish(const gp_Vec &delta);

private slots:
    void slCalibWaitTimeout();

private:
    std::vector <GUI_TYPES::STaskPoint> curTask;
    int lastTaskDelay;
    int calibWaitCounter;
};

#endif // CFANUCBOTSOCKET_H
