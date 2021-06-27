#ifndef CMAINVIEWPORT_H
#define CMAINVIEWPORT_H

#include <QWidget>

#include "gui_types.h"

class QMenu;
class OpenGl_GraphicDriver;
class CMainViewportPrivate;
class Quantity_Color;
class TopoDS_Shape;
class SGuiSettings;
class CInteractiveContext;

class CMainViewport : public QWidget
{
    Q_OBJECT

    friend class CMainViewportPrivate;

public:
    explicit CMainViewport(QWidget *parent = nullptr);
    ~CMainViewport();

    void init(OpenGl_GraphicDriver &driver);
    void setGuiSettings(const SGuiSettings &settings);
    SGuiSettings getGuiSettings() const;

    void setMSAA(const GUI_TYPES::TMSAA msaa);
    GUI_TYPES::TMSAA getMSAA() const;
    void setStatsVisible(const bool value);
    void setShading(const bool enabled);
    void fitInView();
    void setCoord(const GUI_TYPES::TCoordSystem type);

    void setUserAction(const GUI_TYPES::TUsrAction usrAction);
    GUI_TYPES::TUsrAction getUsrAction() const;

    void setPartModel(const TopoDS_Shape &shape);
    void setDeskModel(const TopoDS_Shape &shape);
    void setGripModel(const TopoDS_Shape &shape);

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

private slots:
    void slAddCalibPoint();
    void slChangeCalibPoint();
    void slRemoveCalibPoint();

    void slAddTaskPoint();
    void slChangeTaskPoint();
    void slRemoveTaskPoint();

private:
    CMainViewportPrivate * const d_ptr;
};

#endif // CMAINVIEWPORT_H
