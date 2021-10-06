#ifndef CADVANCEDDEPTHMAPVIEWPORT_H
#define CADVANCEDDEPTHMAPVIEWPORT_H

#include "cadvancedviewport.h"

class CAdvancedDepthMapViewportPrivate;

class CAdvancedDepthMapViewport : public CAdvancedViewport
{
public:
    CAdvancedDepthMapViewport(QWidget *parent = nullptr);
    ~CAdvancedDepthMapViewport();

protected:
    void initPrivate(AIS_InteractiveContext &context) final;

    bool modelShapeChangedPrivate(AIS_InteractiveContext &context,
                                  const GUI_TYPES::EN_ShapeType model,
                                  const TopoDS_Shape &shape) final;
    bool modelTransformChangedPrivate(AIS_InteractiveContext &context,
                                      const GUI_TYPES::EN_ShapeType model,
                                      const gp_Trsf &trsf) final;

private:
    CAdvancedDepthMapViewportPrivate * const d_ptr;
};

#endif // CADVANCEDDEPTHMAPVIEWPORT_H
