#include "cabstractbotsocket.h"

#include <TopoDS_Shape.hxx>

#include "cabstractui.h"

static class CEmptyUi : public CAbstractUi
{
    friend class CAbstractBotSocket;

protected:
    void socketStateChanged(const BotSocket::EN_BotState) final { }
    void laserHeadPositionChanged(const BotSocket::SBotPosition &) final { }
    void gripPositionChanged(const BotSocket::SBotPosition &) final { }
    GUI_TYPES::EN_UiStates getUiState() const final { return GUI_TYPES::ENUS_TASK_EDITING; }
    const TopoDS_Shape& getShape(const BotSocket::EN_ShapeType) const final { return shape; }
    std::vector <GUI_TYPES::SCalibPoint> getCalibPoints() const final {
        return std::vector <GUI_TYPES::SCalibPoint> ();
    }
    std::vector <GUI_TYPES::STaskPoint> getTaskPoints() const final {
        return std::vector <GUI_TYPES::STaskPoint> ();
    }

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

GUI_TYPES::EN_UiStates CAbstractBotSocket::getUiState() const
{
    return ui->getUiState();
}

const TopoDS_Shape &CAbstractBotSocket::getShape(const BotSocket::EN_ShapeType shType) const
{
    return ui->getShape(shType);
}

std::vector<GUI_TYPES::SCalibPoint> CAbstractBotSocket::getCalibPoints() const
{
    return ui->getCalibPoints();
}

std::vector<GUI_TYPES::STaskPoint> CAbstractBotSocket::getTaskPoints() const
{
    return ui->getTaskPoints();
}
