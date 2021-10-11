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

    bool modelShapeChangedPrivate(AIS_InteractiveContext &,
                                  const GUI_TYPES::EN_ShapeType,
                                  AIS_Shape &) final { return false; }
    bool modelTransformChangedPrivate(AIS_InteractiveContext &,
                                      const GUI_TYPES::EN_ShapeType,
                                      const gp_Trsf &) final { return false; }

private:
    CAdvancedDepthMapViewportPrivate * const d_ptr;
};

#endif // CADVANCEDDEPTHMAPVIEWPORT_H
