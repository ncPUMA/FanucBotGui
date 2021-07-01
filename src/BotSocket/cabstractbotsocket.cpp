#include "cabstractbotsocket.h"

#include <TopoDS_Shape.hxx>

#include "cabstractui.h"

static class CEmptyUi : public CAbstractUi
{
    friend class CAbstractBotSocket;

protected:
    void prepareComplete(const BotSocket::EN_PrepareResult) final { }
    void tasksComplete(const BotSocket::EN_WorkResult) final { }
    void socketStateChanged(const BotSocket::EN_BotState) final { }
    void laserHeadPositionChanged(const BotSocket::SBotPosition &) final { }
    void gripPositionChanged(const BotSocket::SBotPosition &) final { }
    const TopoDS_Shape& getShape(const BotSocket::EN_ShapeType) const final { return shape; }

private:
    TopoDS_Shape shape;

} emptyUi;



CAbstractBotSocket::~CAbstractBotSocket()
{

}

CAbstractBotSocket::CAbstractBotSocket() :
    ui(&emptyUi)
{

}

void CAbstractBotSocket::prepareComplete(const BotSocket::EN_PrepareResult result)
{
    ui->prepareComplete(result);
}

void CAbstractBotSocket::tasksComplete(const BotSocket::EN_WorkResult result)
{
    ui->tasksComplete(result);
}

void CAbstractBotSocket::socketStateChanged(const BotSocket::EN_BotState state)
{
    ui->socketStateChanged(state);
}

void CAbstractBotSocket::laserHeadPositionChanged(const BotSocket::SBotPosition &pos)
{
    ui->laserHeadPositionChanged(pos);
}

void CAbstractBotSocket::gripPositionChanged(const BotSocket::SBotPosition &pos)
{
    ui->gripPositionChanged(pos);
}

const TopoDS_Shape &CAbstractBotSocket::getShape(const BotSocket::EN_ShapeType shType) const
{
    return ui->getShape(shType);
}
