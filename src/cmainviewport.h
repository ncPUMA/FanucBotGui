#ifndef CMAINVIEWPORT_H
#define CMAINVIEWPORT_H

#include <QWidget>

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

class CMainViewport : public QWidget
{
    Q_OBJECT

    friend class CMainViewportPrivate;

public:
    explicit CMainViewport(QWidget *parent = nullptr);
    ~CMainViewport();

    void init(OpenGl_GraphicDriver &driver);
    void setGuiSettings(const GUI_TYPES::SGuiSettings &settings);
    GUI_TYPES::SGuiSettings getGuiSettings() const;

    void setMSAA(const GUI_TYPES::TMSAA msaa);
    GUI_TYPES::TMSAA getMSAA() const;
    void setStatsVisible(const bool value);
    void setShading(const bool enabled);
    void fitInView();
    void setCoord(const GUI_TYPES::TCoordSystem type);

    void setUserAction(const GUI_TYPES::EN_UserActions usrAction);
    GUI_TYPES::EN_UserActions getUsrAction() const;

    void setPartModel(const TopoDS_Shape &shape);
    void setDeskModel(const TopoDS_Shape &shape);
    void setLsrheadModel(const TopoDS_Shape &shape);
    void setGripModel(const TopoDS_Shape &shape);

    const TopoDS_Shape& getPartShape() const;
    const TopoDS_Shape& getDeskShape() const;
    const TopoDS_Shape& getLsrheadShape() const;
    const TopoDS_Shape& getGripShape() const;

    void setBotState(const BotSocket::EN_BotState state);
    BotSocket::EN_BotState getBotState() const;
    void moveLsrhead(const BotSocket::SBotPosition &pos);
    void moveGrip(const BotSocket::SBotPosition &pos);

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
