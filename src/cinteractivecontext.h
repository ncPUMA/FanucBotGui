#ifndef CINTERACTIVECONTEXT_H
#define CINTERACTIVECONTEXT_H

#include <Graphic3d_ZLayerId.hxx>

namespace GUI_TYPES {
struct SCalibPoint;
struct STaskPoint;
}

class CInteractiveContextPrivate;
class AIS_InteractiveContext;
class AIS_InteractiveObject;
class TopoDS_Shape;
class gp_Trsf;
class gp_Pnt;
class gp_Dir;

class CInteractiveContext
{
public:
    CInteractiveContext();
    virtual ~CInteractiveContext();

    AIS_InteractiveContext& context();

    void setDisableTepthTestZLayer(const Graphic3d_ZLayerId zLayerWithoutDepthTest);
    void init(AIS_InteractiveContext &context);

    void setShading(const bool enabled);

    void updateCursorPosition();
    void resetCursorPosition();
    gp_Pnt lastCursorPosition() const;

    void setPartModel(const TopoDS_Shape &shape);
    void setPartMdlTransform(const gp_Trsf &trsf);
    void setDeskModel(const TopoDS_Shape &shape);
    void setDeskMdlTransform(const gp_Trsf &trsf);
    void setLsrheadModel(const TopoDS_Shape &shape);
    void setLsrheadMdlTransform(const gp_Trsf &trsf);
    void setLaserLine(const gp_Pnt &pnt, const gp_Dir &dir,
                      const double lenght, const bool clipping);
    void setGripModel(const TopoDS_Shape &shape);
    void setGripMdlTransform(const gp_Trsf &trsf);

    const TopoDS_Shape& getPartShape() const;
    const TopoDS_Shape& getDeskShape() const;
    const TopoDS_Shape& getLsrHeadShape() const;
    const TopoDS_Shape& getGripShape() const;

    void hideAllAdditionalObjects();
    void showCalibObjects();
    void showTaskObjects();

    void setGripVisible(const bool enabled);

    bool isPartDetected() const;
    bool isCalibPointDetected(size_t &index) const;
    bool isTaskPointDetected(size_t &index) const;

    //Usr points
    size_t getCalibPointCount() const;
    GUI_TYPES::SCalibPoint getCalibPoint(const size_t index) const;
    void appendCalibPoint(const GUI_TYPES::SCalibPoint &calibPoint);
    void changeCalibPoint(const size_t index, const GUI_TYPES::SCalibPoint &calibPoint);
    void removeCalibPoint(const size_t index);

    size_t getTaskPointCount() const;
    GUI_TYPES::STaskPoint getTaskPoint(const size_t index) const;
    void appendTaskPoint(const GUI_TYPES::STaskPoint &taskPoint);
    void changeTaskPoint(const size_t index, const GUI_TYPES::STaskPoint &taskPoint);
    void removeTaskPoint(const size_t index);

private:
    CInteractiveContextPrivate * const d_ptr;
};

#endif // CINTERACTIVECONTEXT_H
