#ifndef CMAINVIEWPORT_H
#define CMAINVIEWPORT_H

#include <QWidget>
#include <gp_Trsf.hxx>

#include "BotSocket/bot_socket_types.h"

class QMenu;
class OpenGl_GraphicDriver;
class CMainViewportPrivate;
class Quantity_Color;
class TopoDS_Shape;
class CInteractiveContext;

namespace GUI_TYPES {
class SGuiSettings;
}

class CAbstractMainViewportSubscriber
{
    friend class CMainViewport;

public:
    virtual ~CAbstractMainViewportSubscriber() { }

protected:
    CAbstractMainViewportSubscriber() { }

    virtual void uiStateChanged() = 0;
    virtual void calibrationChanged() = 0;
    virtual void tasksChanged() = 0;
    virtual void pathPointsChanged() = 0;

private:
    CAbstractMainViewportSubscriber(const CAbstractMainViewportSubscriber &) = delete;
    CAbstractMainViewportSubscriber& operator =(const CAbstractMainViewportSubscriber &) = delete;
};


class CMainViewport : public QWidget
{
    Q_OBJECT

    friend class CMainViewportPrivate;

public:
    explicit CMainViewport(QWidget *parent = nullptr);
    ~CMainViewport();

    void addSubscriber(CAbstractMainViewportSubscriber * const subscriber);
    void clearSubscribers();
    void init(OpenGl_GraphicDriver &driver);
    void setGuiSettings(const GUI_TYPES::SGuiSettings &settings);
    GUI_TYPES::SGuiSettings getGuiSettings() const;

    void setMSAA(const GUI_TYPES::TMSAA msaa);
    GUI_TYPES::TMSAA getMSAA() const;
    void setSnapshotParams(const GUI_TYPES::TScale scale,
                           const size_t width, const size_t height);
    GUI_TYPES::TScale getSnapshotScale() const;
    void setStatsVisible(const bool value);
    void setShading(const bool enabled);
    void fitInView();
    void setCoord(const GUI_TYPES::TCoordSystem type);

    void setUiState(const GUI_TYPES::EN_UiStates state);
    GUI_TYPES::EN_UiStates getUiState() const;

    void setPartModel(const TopoDS_Shape &shape);
    void setDeskModel(const TopoDS_Shape &shape);
    void setLsrheadModel(const TopoDS_Shape &shape);
    void setGripModel(const TopoDS_Shape &shape);

    const TopoDS_Shape& getPartShape() const;
    const TopoDS_Shape& getDeskShape() const;
    const TopoDS_Shape& getLsrheadShape() const;
    const TopoDS_Shape& getGripShape() const;

    const gp_Trsf& getPartTransform() const;
    const gp_Trsf& getDeskTransform() const;
    const gp_Trsf& getLsrheadTransform() const;
    const gp_Trsf& getGripTransform() const;

    void setCalibResult(const BotSocket::EN_CalibResult val);
    BotSocket::EN_CalibResult getCalibResult() const;
    void setBotState(const BotSocket::EN_BotState state);
    BotSocket::EN_BotState getBotState() const;
    void moveLsrhead(const BotSocket::SBotPosition &pos);
    void moveGrip(const BotSocket::SBotPosition &pos);

    void shapeCalibrationChanged(const BotSocket::EN_ShapeType shType, const BotSocket::SBotPosition &pos);
    void shapeTransformChanged(const BotSocket::EN_ShapeType shType, const gp_Trsf &transform);

    void setCalibrationPoints(const std::vector <GUI_TYPES::SCalibPoint> &points);
    std::vector <GUI_TYPES::SCalibPoint> getCallibrationPoints() const;
    std::vector <GUI_TYPES::SCalibPoint> getCallibrationLocalPoints() const;
    void setTaskPoints(const std::vector <GUI_TYPES::STaskPoint> &points);
    std::vector <GUI_TYPES::STaskPoint> getTaskPoints() const;
    void setPathPoints(const std::vector <GUI_TYPES::SPathPoint> &points);
    std::vector <GUI_TYPES::SPathPoint> getPathPoints() const;

    GUI_TYPES::SGuiSettings partPrntScr();
    void makePartSnapshot(const char *fname);

protected:
    QPaintEngine* paintEngine() const final;
    void paintEvent(QPaintEvent *) final;
    void resizeEvent(QResizeEvent*) final;

    void mousePressEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void wheelEvent(QWheelEvent *event) final;

private:
    QString taskName(const GUI_TYPES::TBotTaskType taskType) const;
    void fillCalibCntxtMenu(QMenu &menu);
    void fillTaskAddCntxtMenu(QMenu &menu);
    void taskPointsChanged();
    void pathPointsChanged();

private slots:
    void slAddCalibPoint();
    void slChangeCalibPoint();
    void slRemoveCalibPoint();

    void slAddTaskPoint();
    void slChangeTaskPoint();
    void slRemoveTaskPoint();

    void slAddPathPoint();
    void slChangePathPoint();
    void slRemovePathPoint();

private:
    CMainViewportPrivate * const d_ptr;
};

#endif // CMAINVIEWPORT_H
