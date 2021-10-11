#ifndef CADVANCEDVIEWPORT_H
#define CADVANCEDVIEWPORT_H

#include <QWidget>

#include "gui_types.h"

class OpenGl_GraphicDriver;
class TopoDS_Shape;
class AIS_Shape;
class gp_Trsf;
class gp_Pnt;
class gp_Dir;
class AIS_InteractiveContext;
class V3d_View;
class CAdvancedViewportPrivate;

class CAdvancedViewport : public QWidget
{
    Q_OBJECT

public:
    CAdvancedViewport(QWidget *parent = nullptr);
    ~CAdvancedViewport();

    void init(OpenGl_GraphicDriver &driver);

    void modelShapeChanged(const GUI_TYPES::EN_ShapeType model,
                           const TopoDS_Shape &shape);
    void modelTransformChanged(const GUI_TYPES::EN_ShapeType model,
                               const gp_Trsf &trsf);

    void setCameraScale(const double scale);
    void setCameraPos(const gp_Pnt &pos, const gp_Pnt &dir, const gp_Dir &orient);

    void setLaserPos(const gp_Pnt &pos, const gp_Dir &dir);

    void createSnapshot(const char *fname, const size_t width, const size_t height);

    QImage createSnapshot(const size_t width, const size_t height);

    void setShapeVisible(const GUI_TYPES::EN_ShapeType model, bool visible);

protected:
    // QWidget
    QPaintEngine* paintEngine() const final;
    void paintEvent(QPaintEvent *) final;
    void resizeEvent(QResizeEvent*) final;

    void mousePressEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void wheelEvent(QWheelEvent *event) final;

    // CAdvancedViewport
    virtual void initPrivate(AIS_InteractiveContext &context, V3d_View &view) = 0;

    virtual bool modelShapeChangedPrivate(AIS_InteractiveContext &context,
                                          const GUI_TYPES::EN_ShapeType model,
                                          AIS_Shape &shape) = 0;
    virtual bool modelTransformChangedPrivate(AIS_InteractiveContext &context,
                                              const GUI_TYPES::EN_ShapeType model,
                                              const gp_Trsf &trsf) = 0;
    virtual void cameraPosChanged(const gp_Pnt &pos, const gp_Pnt &dir, const gp_Dir &orient) = 0;

private:
    CAdvancedViewportPrivate * const d_ptr;
};

#endif // CADVANCEDVIEWPORT_H
