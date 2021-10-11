#include "cabstractbotsocket.h"

#include <QImage>

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
    const TopoDS_Shape& getShape(const GUI_TYPES::EN_ShapeType) const final { return shape; }
    const gp_Trsf getShapeTransform(const GUI_TYPES::EN_ShapeType) const final { return transform; }
    void shapeCalibrationChanged(const GUI_TYPES::EN_ShapeType, const BotSocket::SBotPosition &) final {}
    void shapeTransformChanged(const GUI_TYPES::EN_ShapeType, const gp_Trsf &) final {}

    void setSnapshotCameraPos(const gp_Pnt &, const gp_Pnt &, const gp_Dir &) final { }
    void makeSnapshot(const char *) final { }
    QImage makeSnapshot() final { return QImage(); }
    void setSnapshotShapeVisible(const GUI_TYPES::EN_ShapeType, bool) final { }

    void setDepthMapCameraPos(const gp_Pnt &, const gp_Pnt &, const gp_Dir &) final { }
    void makeDepthMap(const char *) final { }
    QImage makeDepthMap() final { return QImage(); }
    void setDepthMapShapeVisible(const GUI_TYPES::EN_ShapeType, bool) final { }

    void snapshotCalibrationDataRecieved(const gp_Vec &) final { }
    bool execSnapshotCalibrationWarning() final { return false; }
private:
    TopoDS_Shape shape;
    gp_Trsf transform;
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

void CAbstractBotSocket::shapeCalibrationChanged(const GUI_TYPES::EN_ShapeType shType, const BotSocket::SBotPosition &pos)
{
    ui->shapeCalibrationChanged(shType, pos);
}

void CAbstractBotSocket::shapeTransformChanged(const GUI_TYPES::EN_ShapeType shType, const gp_Trsf &transform)
{
    ui->shapeTransformChanged(shType, transform);
}

const TopoDS_Shape &CAbstractBotSocket::getShape(const GUI_TYPES::EN_ShapeType shType) const
{
    return ui->getShape(shType);
}

const gp_Trsf CAbstractBotSocket::getShapeTransform(const GUI_TYPES::EN_ShapeType shType) const
{
    return ui->getShapeTransform(shType);
}

void CAbstractBotSocket::setSnapshotCameraPos(const gp_Pnt &pos, const gp_Pnt &dir, const gp_Dir &orient)
{
    ui->setSnapshotCameraPos(pos, dir, orient);
}

void CAbstractBotSocket::makeSnapshot(const char *fname)
{
    ui->makeSnapshot(fname);
}

QImage CAbstractBotSocket::makeSnapshot()
{
    return ui->makeSnapshot();
}

void CAbstractBotSocket::setSnapshotShapeVisible(const GUI_TYPES::EN_ShapeType model, bool visible)
{
    ui->setSnapshotShapeVisible(model, visible);
}

void CAbstractBotSocket::setDepthMapCameraPos(const gp_Pnt &pos, const gp_Pnt &dir, const gp_Dir &orient)
{
    ui->setDepthMapCameraPos(pos, dir, orient);
}

void CAbstractBotSocket::makeDepthMap(const char *fname)
{
    ui->makeDepthMap(fname);
}

QImage CAbstractBotSocket::makeDepthMap()
{
    return ui->makeDepthMap();
}

void CAbstractBotSocket::setDepthMapShapeVisible(const GUI_TYPES::EN_ShapeType model, bool visible)
{
    ui->setDepthMapShapeVisible(model, visible);
}

void CAbstractBotSocket::snapshotCalibrationDataRecieved(const gp_Vec &globalDelta)
{
    ui->snapshotCalibrationDataRecieved(globalDelta);
}

bool CAbstractBotSocket::execSnapshotCalibrationWarning()
{
    return ui->execSnapshotCalibrationWarning();
}
