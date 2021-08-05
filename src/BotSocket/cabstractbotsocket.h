#ifndef CABSTRACTBOTSOCKET_H
#define CABSTRACTBOTSOCKET_H

#include <vector>
#include <gp_Trsf.hxx>

#include "bot_socket_types.h"

class CAbstractUi;

class TopoDS_Shape;

class CAbstractBotSocket
{
    friend class CAbstractUi;

public:
    virtual ~CAbstractBotSocket();

protected:
    CAbstractBotSocket();

    virtual BotSocket::EN_CalibResult execCalibration(const std::vector <GUI_TYPES::SCalibPoint> &points) = 0;
    virtual void prepare(const std::vector <GUI_TYPES::STaskPoint> &points) = 0;
    virtual void startTasks(const std::vector <GUI_TYPES::STaskPoint> &points) = 0;
    virtual void stopTasks() = 0;
    virtual void shapeTransformChanged(const BotSocket::EN_ShapeType shType) = 0;

    void prepareComplete(const BotSocket::EN_PrepareResult result);
    void tasksComplete(const BotSocket::EN_WorkResult result);
    void socketStateChanged(const BotSocket::EN_BotState state);
    void laserHeadPositionChanged(const BotSocket::SBotPosition &pos);
    void gripPositionChanged(const BotSocket::SBotPosition &pos);
    void shapeCalibrationChanged(const BotSocket::EN_ShapeType shType, const BotSocket::SBotPosition &pos);
    void shapeTransformChanged(const BotSocket::EN_ShapeType shType, const gp_Trsf &transform);
    GUI_TYPES::EN_UiStates getUiState() const;
    const TopoDS_Shape& getShape(const BotSocket::EN_ShapeType shType) const;

private:
    CAbstractBotSocket(const CAbstractBotSocket &) = delete;
    CAbstractBotSocket& operator=(const CAbstractBotSocket &) = delete;

private:
    CAbstractUi *ui;
};

#endif // CABSTRACTBOTSOCKET_H
