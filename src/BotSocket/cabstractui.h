#ifndef CABSTRACTUI_H
#define CABSTRACTUI_H

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
    virtual const TopoDS_Shape& getShape(const BotSocket::EN_ShapeType shType) const = 0;

    void init(const GUI_TYPES::EN_UserActions curAction,
              const std::map <BotSocket::EN_ShapeType, TopoDS_Shape> &shapes);
    void shapeTransformChaged(const BotSocket::EN_ShapeType shType);
    void usrActionChanged(const GUI_TYPES::EN_UserActions action);

private:
    CAbstractUi(const CAbstractUi &) = delete;
    CAbstractUi& operator =(const CAbstractUi &) = delete;

private:
    CAbstractUiPrivate * const d_ptr;
};

#endif // CABSTRACTUI_H
