#include "cabstractui.h"

#include "cabstractbotsocket.h"

static class CEmptyBotSocket : public CAbstractBotSocket
{
public:
    CEmptyBotSocket() : CAbstractBotSocket() { }

protected:
    BotSocket::EN_CalibResult execCalibration(const std::vector <GUI_TYPES::SCalibPoint> &) final {
        return BotSocket::ENCR_FALL;
    }
    void prepare(const std::vector <GUI_TYPES::STaskPoint> &) final { }
    void startTasks(const std::vector <GUI_TYPES::SHomePoint> &,
                    const std::vector <GUI_TYPES::STaskPoint> &) final { }
    void stopTasks() final { }
    void shapeTransformChanged(const BotSocket::EN_ShapeType) final { }

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

BotSocket::EN_CalibResult CAbstractUi::execCalibration(const std::vector<GUI_TYPES::SCalibPoint> &points)
{
    return d_ptr->bot->execCalibration(points);
}

void CAbstractUi::prepare(const std::vector<GUI_TYPES::STaskPoint> &points)
{
    d_ptr->bot->prepare(points);
}

void CAbstractUi::startTasks(const std::vector<GUI_TYPES::SHomePoint> &homePoints,
                             const std::vector<GUI_TYPES::STaskPoint> &taskPoints)
{
    d_ptr->bot->startTasks(homePoints, taskPoints);
}

void CAbstractUi::stopTasks()
{
    d_ptr->bot->stopTasks();
}

void CAbstractUi::shapeTransformChaged(const BotSocket::EN_ShapeType shType)
{
    d_ptr->bot->shapeTransformChanged(shType);
}

