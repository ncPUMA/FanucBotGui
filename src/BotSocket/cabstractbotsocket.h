#ifndef CABSTRACTBOTSOCKET_H
#define CABSTRACTBOTSOCKET_H

#include <vector>

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

    virtual void uiStateChanged(const GUI_TYPES::EN_UiStates state) = 0;
    virtual void shapeTransformChaged(const BotSocket::EN_ShapeType shType) = 0;

    void socketStateChanged(const BotSocket::TBotState state);
    void laserHeadPositionChanged(const BotSocket::SBotPosition &pos);
    void gripPositionChanged(const BotSocket::SBotPosition &pos);
    GUI_TYPES::EN_UiStates getUiState() const;
    const TopoDS_Shape& getShape(const BotSocket::EN_ShapeType shType) const;
    std::vector <GUI_TYPES::SCalibPoint> getCalibPoints() const;
    std::vector <GUI_TYPES::STaskPoint> getTaskPoints() const;

private:
    CAbstractBotSocket(const CAbstractBotSocket &) = delete;
    CAbstractBotSocket& operator=(const CAbstractBotSocket &) = delete;

private:
    CAbstractUi *ui;
};

#endif // CABSTRACTBOTSOCKET_H
