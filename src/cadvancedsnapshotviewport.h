#ifndef CADVANCEDSNAPSHOTVIEWPORT_H
#define CADVANCEDSNAPSHOTVIEWPORT_H

#include "cadvancedviewport.h"

class CAdvancedSnapshotViewportPrivate;

class CAdvancedSnapshotViewport : public CAdvancedViewport
{
public:
    CAdvancedSnapshotViewport(QWidget *parent = nullptr);
    ~CAdvancedSnapshotViewport();

protected:
    void initPrivate(AIS_InteractiveContext &context, V3d_View &view) final;

    bool modelShapeChangedPrivate(AIS_InteractiveContext &context,
                                  const GUI_TYPES::EN_ShapeType model,
                                  AIS_Shape &shape) final;
    bool modelTransformChangedPrivate(AIS_InteractiveContext &,
                                      const GUI_TYPES::EN_ShapeType,
                                      const gp_Trsf &) final { return false; }
    void cameraPosChanged(const gp_Pnt &pos, const gp_Pnt &dir, const gp_Dir &orient) final;

private:
    CAdvancedSnapshotViewportPrivate * const d_ptr;
};

#endif // CADVANCEDSNAPSHOTVIEWPORT_H
