#ifndef CMAINVIEWPORT_H
#define CMAINVIEWPORT_H

#include <QWidget>

#include "gui_types.h"

class CMainViewportPrivate;
class AIS_InteractiveContext;
class Quantity_Color;

class CMainViewport : public QWidget
{
    Q_OBJECT
public:
    explicit CMainViewport(QWidget *parent = nullptr);
    ~CMainViewport();

    void init(AIS_InteractiveContext &context);

    void setMSAA(const GUI_TYPES::TMSAA msaa);
    void setStatsVisible(const bool value);
    void fitInView();
    void setBackgroundColor(const Quantity_Color &clr);
    void setCoord(const GUI_TYPES::TCoordSystem type);

protected:
    QPaintEngine* paintEngine() const final;
    void paintEvent(QPaintEvent *) final;
    void resizeEvent(QResizeEvent*) final;

    void mousePressEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void wheelEvent(QWheelEvent *event) final;

private:
    CMainViewportPrivate * const d_ptr;
};

#endif // CMAINVIEWPORT_H
