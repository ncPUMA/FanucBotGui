#include "cabstractui.h"

#include "cabstractbotsocket.h"

static class CEmptyBotSocket : public CAbstractBotSocket
{
public:
    CEmptyBotSocket() : CAbstractBotSocket() { }

protected:
    void init(const GUI_TYPES::EN_UserActions,
              const std::map <BotSocket::EN_ShapeType, TopoDS_Shape> &) final { }
    void shapeTransformChaged(const BotSocket::EN_ShapeType) final { }
    void usrActionChanged(const GUI_TYPES::EN_UserActions) final { }
} emptySocket;


class CAbstractUiPrivate
{
    friend class CAbstractUi;

    CAbstractUiPrivate() :
        bot(&emptySocket) { }

    CAbstractBotSocket *bot;
};


CAbstractUi::~CAbstractUi()
{
    delete d_ptr;
}

CAbstractUi::CAbstractUi() :
    d_ptr(new CAbstractUiPrivate())
{

}

void CAbstractUi::setBotSocket(CAbstractBotSocket &socket)
{
    d_ptr->bot = &socket;
    d_ptr->bot->ui = this;
}

void CAbstractUi::init(const GUI_TYPES::EN_UserActions curAction,
                       const std::map<BotSocket::EN_ShapeType, TopoDS_Shape> &shapes)
{
    d_ptr->bot->init(curAction, shapes);
}

void CAbstractUi::shapeTransformChaged(const BotSocket::EN_ShapeType shType)
{
    d_ptr->bot->shapeTransformChaged(shType);
}

void CAbstractUi::usrActionChanged(const GUI_TYPES::EN_UserActions action)
{
    d_ptr->bot->usrActionChanged(action);
}
