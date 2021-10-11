#ifndef CABSTRACTUI_H
#define CABSTRACTUI_H

#include <vector>
#include <gp_Trsf.hxx>

#include "bot_socket_types.h"

class QImage;

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

    virtual void prepareComplete(const BotSocket::EN_PrepareResult result) = 0;
    virtual void tasksComplete(const BotSocket::EN_WorkResult result) = 0;
    virtual void socketStateChanged(const BotSocket::EN_BotState state) = 0;
    virtual void laserHeadPositionChanged(const BotSocket::SBotPosition &pos) = 0;
    virtual void gripPositionChanged(const BotSocket::SBotPosition &pos) = 0;
    virtual void shapeCalibrationChanged(const GUI_TYPES::EN_ShapeType shType, const BotSocket::SBotPosition &pos) = 0;
    virtual void shapeTransformChanged(const GUI_TYPES::EN_ShapeType shType, const gp_Trsf &transform) = 0;
    virtual const TopoDS_Shape& getShape(const GUI_TYPES::EN_ShapeType shType) const = 0;
    virtual const gp_Trsf getShapeTransform(const GUI_TYPES::EN_ShapeType shType) const = 0;

    BotSocket::EN_CalibResult execCalibration(const std::vector <GUI_TYPES::SCalibPoint> &points);
    void prepare(const std::vector <GUI_TYPES::STaskPoint> &points);
    void startTasks(const std::vector<GUI_TYPES::SHomePoint> &homePoints,
                    const std::vector <GUI_TYPES::STaskPoint> &taskPoints);
    void stopTasks();
    void shapeTransformChaged(const GUI_TYPES::EN_ShapeType shType);

    //
    virtual void setSnapshotCameraPos(const gp_Pnt &pos, const gp_Pnt &dir, const gp_Dir &orient) = 0;
    virtual void makeSnapshot(const char *fname) = 0;
    virtual QImage makeSnapshot() = 0;
    virtual void setSnapshotShapeVisible(const GUI_TYPES::EN_ShapeType model, bool visible) = 0;

    virtual void setDepthMapCameraPos(const gp_Pnt &pos, const gp_Pnt &dir, const gp_Dir &orient) = 0;
    virtual void makeDepthMap(const char *fname) = 0;
    virtual QImage makeDepthMap() = 0;
    virtual void setDepthMapShapeVisible(const GUI_TYPES::EN_ShapeType model, bool visible) = 0;

    virtual void snapshotCalibrationDataRecieved(const gp_Vec &globalDelta) = 0;
    virtual bool execSnapshotCalibrationWarning() = 0;

private:
    CAbstractUi(const CAbstractUi &) = delete;
    CAbstractUi& operator =(const CAbstractUi &) = delete;

private:
    CAbstractUiPrivate * const d_ptr;
};

#endif // CABSTRACTUI_H
