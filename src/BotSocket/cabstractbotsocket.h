#ifndef CABSTRACTBOTSOCKET_H
#define CABSTRACTBOTSOCKET_H

#include <vector>
#include <gp_Trsf.hxx>

#include "bot_socket_types.h"

class QImage;

class CAbstractUi;

class TopoDS_Shape;

class CAbstractBotSocket
{
    friend class CAbstractUi;

public:
    virtual ~CAbstractBotSocket();

protected:
    CAbstractBotSocket();

    virtual BotSocket::EN_CalibResult execCalibration(const std::vector <GUI_TYPES::SCalibPoint> &points) = 0;
    virtual void prepare(const std::vector <GUI_TYPES::STaskPoint> &points) = 0;
    virtual void startTasks(const std::vector <GUI_TYPES::SHomePoint> &homePoints,
                            const std::vector <GUI_TYPES::STaskPoint> &taskPoints) = 0;
    virtual void stopTasks() = 0;
    virtual void shapeTransformChanged(const GUI_TYPES::EN_ShapeType shType) = 0;

    void prepareComplete(const BotSocket::EN_PrepareResult result);
    void tasksComplete(const BotSocket::EN_WorkResult result);
    void socketStateChanged(const BotSocket::EN_BotState state);
    void laserHeadPositionChanged(const BotSocket::SBotPosition &pos);
    void gripPositionChanged(const BotSocket::SBotPosition &pos);
    void shapeCalibrationChanged(const GUI_TYPES::EN_ShapeType shType, const BotSocket::SBotPosition &pos);
    void shapeTransformChanged(const GUI_TYPES::EN_ShapeType shType, const gp_Trsf &transform);
    GUI_TYPES::EN_UiStates getUiState() const;
    const TopoDS_Shape& getShape(const GUI_TYPES::EN_ShapeType shType) const;
    const gp_Trsf getShapeTransform(const GUI_TYPES::EN_ShapeType shType) const;

    void setSnapshotCameraPos(const gp_Pnt &pos, const gp_Pnt &dir, const gp_Dir &orient);
    void makeSnapshot(const char *fname);
    QImage makeSnapshot();
    void setSnapshotShapeVisible(const GUI_TYPES::EN_ShapeType model, bool visible);

    void setDepthMapCameraPos(const gp_Pnt &pos, const gp_Pnt &dir, const gp_Dir &orient);
    void makeDepthMap(const char *fname);
    QImage makeDepthMap();
    void setDepthMapShapeVisible(const GUI_TYPES::EN_ShapeType model, bool visible);

    void snapshotCalibrationDataRecieved(const gp_Vec &globalDelta);
    bool execSnapshotCalibrationWarning();

private:
    CAbstractBotSocket(const CAbstractBotSocket &) = delete;
    CAbstractBotSocket& operator=(const CAbstractBotSocket &) = delete;

private:
    CAbstractUi *ui;
};

#endif // CABSTRACTBOTSOCKET_H
