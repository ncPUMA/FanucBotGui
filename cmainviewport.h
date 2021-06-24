#ifndef CMAINVIEWPORT_H
#define CMAINVIEWPORT_H

#include <QWidget>

#include "gui_types.h"

class OpenGl_GraphicDriver;
class CMainViewportPrivate;
class Quantity_Color;
class TopoDS_Shape;
class SGuiSettings;

class CMainViewport : public QWidget
{
    Q_OBJECT

public:
    explicit CMainViewport(QWidget *parent = nullptr);
    ~CMainViewport();

    void init(OpenGl_GraphicDriver &driver);
    void setGuiSettings(const SGuiSettings &settings);
    SGuiSettings getGuiSettings() const;

    void setMSAA(const GUI_TYPES::TMSAA msaa);
    GUI_TYPES::TMSAA getMSAA() const;
    void setStatsVisible(const bool value);
    void fitInView();
    void setCoord(const GUI_TYPES::TCoordSystem type);

    void setCalibEnabled(bool enabled);

    void setMainModel(const TopoDS_Shape &shape);
    void setGripModel(const TopoDS_Shape &shape);

    void updateModelsDefaultPosition(const bool shading);

protected:
    QPaintEngine* paintEngine() const final;
    void paintEvent(QPaintEvent *) final;
    void resizeEvent(QResizeEvent*) final;

    void mousePressEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void wheelEvent(QWheelEvent *event) final;

private slots:
    void slAddCalibPoint();
    void slChangeCalibPoint();
    void slRemoveCalibPoint();

private:
    CMainViewportPrivate * const d_ptr;
};

#endif // CMAINVIEWPORT_H
