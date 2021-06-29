#ifndef CABSTRACTBOTSOCKET_H
#define CABSTRACTBOTSOCKET_H

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

    void socketStateChanged(const BotSocket::TBotState state);
    void laserHeadPositionChanged(const BotSocket::SBotPosition &pos);
    void gripPositionChanged(const BotSocket::SBotPosition &pos);

    virtual void init(const GUI_TYPES::EN_UserActions curAction,
                      const std::map <BotSocket::EN_ShapeType, TopoDS_Shape> &shapes) = 0;
    virtual void shapeTransformChaged(const BotSocket::EN_ShapeType shType,
                                      const TopoDS_Shape &shape) = 0;
    virtual void usrActionChanged(const GUI_TYPES::EN_UserActions action) = 0;


private:
    CAbstractBotSocket(const CAbstractBotSocket &) = delete;
    CAbstractBotSocket& operator=(const CAbstractBotSocket &) = delete;

private:
    CAbstractUi *ui;
};

#endif // CABSTRACTBOTSOCKET_H
