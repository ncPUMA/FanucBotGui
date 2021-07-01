#ifndef CABSTRACTUI_H
#define CABSTRACTUI_H

#include <vector>

#include "bot_socket_types.h"

class CAbstractUiPrivate;
class CAbstractBotSocket;

class TopoDS_Shape;

class CAbstractUi
{
    friend class CAbstractBotSocket;

public:
    virtual ~CAbstractUi();

protected:
    CAbstractUi();

    void setBotSocket(CAbstractBotSocket &socket);

    virtual void socketStateChanged(const BotSocket::TBotState state) = 0;
    virtual void laserHeadPositionChanged(const BotSocket::SBotPosition &pos) = 0;
    virtual void gripPositionChanged(const BotSocket::SBotPosition &pos) = 0;
    virtual GUI_TYPES::EN_UiStates getUiState() const = 0;
    virtual const TopoDS_Shape& getShape(const BotSocket::EN_ShapeType shType) const = 0;
    virtual std::vector <GUI_TYPES::SCalibPoint> getCalibPoints() const = 0;
    virtual std::vector <GUI_TYPES::STaskPoint> getTaskPoints() const = 0;

    void uiStateChanged(const GUI_TYPES::EN_UiStates state);
    void shapeTransformChaged(const BotSocket::EN_ShapeType shType);

private:
    CAbstractUi(const CAbstractUi &) = delete;
    CAbstractUi& operator =(const CAbstractUi &) = delete;

private:
    CAbstractUiPrivate * const d_ptr;
};

#endif // CABSTRACTUI_H
