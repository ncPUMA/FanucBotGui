#include "cabstractui.h"

#include "cabstractbotsocket.h"

static class CEmptyBotSocket : public CAbstractBotSocket
{
public:
    CEmptyBotSocket() : CAbstractBotSocket() { }

protected:
    void uiStateChanged(const GUI_TYPES::EN_UiStates) final { }
    void shapeTransformChaged(const BotSocket::EN_ShapeType) final { }

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

void CAbstractUi::uiStateChanged(const GUI_TYPES::EN_UiStates state)
{
    d_ptr->bot->uiStateChanged(state);
}

void CAbstractUi::shapeTransformChaged(const BotSocket::EN_ShapeType shType)
{
    d_ptr->bot->shapeTransformChaged(shType);
}
