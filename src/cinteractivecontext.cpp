#include "cinteractivecontext.h"

#include <string>
#include <sstream>
#include <cassert>
#include <sstream>
#include <map>

#include <AIS_InteractiveContext.hxx>

#include <Standard_Version.hxx>

#include <AIS_Trihedron.hxx>
#include <AIS_Point.hxx>
#include <AIS_TextLabel.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Shape.hxx>

#include <Geom_CartesianPoint.hxx>
#include <Geom_Axis2Placement.hxx>

#include <TopExp_Explorer.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <TopoDS.hxx>

#include "gui_types.h"

#include "Primitives/claservec.h"
#include "Primitives/ctaskpnt.h"

static const Quantity_Color TXT_CLR  = Quantity_Color( .15  ,  .15, 0.15, Quantity_TOC_RGB);
static const Quantity_Color FACE_CLR = Quantity_Color(0.1   , 0.1 , 0.1 , Quantity_TOC_RGB);

class CInteractiveContextPrivate
{
    friend class CInteractiveContext;

private:
    CInteractiveContextPrivate() :
        depthTestOffZlayer(Graphic3d_ZLayerId_UNKNOWN),
        bShading(false),
        lsrClip(true),
        bCursorIsVisible(false),
        cursorPnt(new AIS_Point(new Geom_CartesianPoint(gp_Pnt()))),
        cursorLbl(new AIS_TextLabel())
    { }

    ~CInteractiveContextPrivate() { }

    static AIS_ViewCube* createCube() {
        AIS_ViewCube * const aViewCube = new AIS_ViewCube();
        aViewCube->SetDrawEdges(Standard_False);
        aViewCube->SetDrawVertices(Standard_False);
        aViewCube->SetBoxTransparency(1.);
        aViewCube->AIS_InteractiveObject::SetColor(TXT_CLR);
        TCollection_AsciiString emptyStr;
        aViewCube->SetBoxSideLabel(V3d_Xpos, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Ypos, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Zpos, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Xneg, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Yneg, emptyStr);
        aViewCube->SetBoxSideLabel(V3d_Zneg, emptyStr);
        return aViewCube;
    }

    void init(AIS_InteractiveContext &cntxt) {
        context = &cntxt;
//        context->SetAutoActivateSelection(false);

        Handle(Prs3d_Drawer) drawer = context->DefaultDrawer();
        Handle(Prs3d_DatumAspect) datum = drawer->DatumAspect();

#if OCC_VERSION_HEX >= 0x070600
        datum->TextAspect(Prs3d_DatumParts_XAxis)->SetColor(TXT_CLR);
        datum->TextAspect(Prs3d_DatumParts_YAxis)->SetColor(TXT_CLR);
        datum->TextAspect(Prs3d_DatumParts_ZAxis)->SetColor(TXT_CLR);
#else
        datum->TextAspect()->SetColor(TXT_CLR);
#endif

        Handle(Prs3d_LineAspect) lAspect = drawer->FaceBoundaryAspect();
        lAspect->SetColor(FACE_CLR);
        drawer->SetFaceBoundaryAspect(lAspect);
        drawer->SetFaceBoundaryDraw(Standard_True);

        //Add AIS_ViewCube
        ais_axis_cube = createCube();
        context->Display(ais_axis_cube, Standard_False);
        context->SetDisplayMode(ais_axis_cube, 1, Standard_False);
        context->Deactivate(ais_axis_cube);

        //Add calib. trihedron
        Geom_Axis2Placement coords(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.), gp_Dir(1., 0., 0.));
        Handle(Geom_Axis2Placement) axis = new Geom_Axis2Placement(coords);
        calibTrihedron = new AIS_Trihedron(axis);
        context->Load(calibTrihedron, Standard_False);
        context->SetDisplayMode(calibTrihedron, 1, Standard_False);
        context->SetZLayer(calibTrihedron, depthTestOffZlayer);
        context->Deactivate(calibTrihedron);

        //Add cursor
        context->Load(cursorPnt, Standard_False);
        context->SetZLayer(cursorPnt, depthTestOffZlayer);
        context->Deactivate(cursorPnt);
        context->Load(cursorLbl, Standard_False);
        context->SetZLayer(cursorLbl, depthTestOffZlayer);
        context->Deactivate(cursorLbl);
    }

    void setShading(const bool enabled) {
        bShading = enabled;
        const AIS_DisplayMode mode = bShading ? AIS_Shaded : AIS_WireFrame;
        context->SetDisplayMode(ais_part, mode, Standard_False);
        context->Redisplay(ais_part, Standard_False);
        context->SetDisplayMode(ais_desk, mode, Standard_False);
        context->RecomputePrsOnly(ais_desk, Standard_False);
        context->SetDisplayMode(ais_lsrhead, mode, Standard_False);
        context->RecomputePrsOnly(ais_lsrhead, Standard_False);
        context->SetDisplayMode(ais_grip, mode, Standard_False);
        context->RecomputePrsOnly(ais_grip, Standard_False);
    }

    void updateCursorPosition() {
        const bool bLastVisible = bCursorIsVisible;
        Handle(SelectMgr_EntityOwner) owner = context->DetectedOwner();
        if (owner) {
            Handle(AIS_Shape) shape = Handle(AIS_Shape)::DownCast(owner->Selectable());
            bCursorIsVisible =
                    (shape == ais_part) ||
                    (shape == ais_desk) ||
                    (shape == ais_grip) ||
                    (shape == ais_lsrhead);
        }

        if (bCursorIsVisible) {
            Handle(StdSelect_ViewerSelector3d) selector = context->MainSelector();
            if (selector->NbPicked() > 0) {
                //cross
                const gp_Pnt pick = selector->PickedPoint(1);
                gp_Trsf transform;
                transform.SetTranslation(gp_Pnt(), pick);
                context->SetLocation(cursorPnt, transform);

                //label
                std::stringstream stream;
                stream //<< std::setfill('0') << std::setw(10)
                       << "  X: " << pick.X() << "\n"
                       << "  Y: " << pick.Y() << "\n"
                       << "  Z: " << pick.Z();
                cursorLbl->SetPosition(pick);
                cursorLbl->SetText(stream.str().c_str());
            }
            else
                bCursorIsVisible = false;
        }

        if (bLastVisible) {
            if (bCursorIsVisible) {
                context->RecomputePrsOnly(cursorLbl, Standard_False);
            }
            else {
                context->Erase(cursorPnt, Standard_False);
                context->Erase(cursorLbl, Standard_False);
            }
        }
        else if (bCursorIsVisible) {
            context->Display(cursorPnt, Standard_False);
            context->Deactivate(cursorPnt);
            context->Display(cursorLbl, Standard_False);
            context->Deactivate(cursorLbl);
        }
    }

    void resetCursorPosition() {
        bCursorIsVisible = false;
        context->Erase(cursorPnt, Standard_False);
        context->Erase(cursorLbl, Standard_False);
    }

    void setPartModel(const TopoDS_Shape &shape) {
        if (!ais_part.IsNull())
            context->Remove(ais_part, Standard_False);
        ais_part = new AIS_Shape(shape);
        context->SetDisplayMode(ais_part, bShading ? AIS_Shaded : AIS_WireFrame, Standard_False);
        context->Display(ais_part, Standard_False);
    }

    void setPartMdlTransform(const gp_Trsf &trsf) {
        if (!ais_part.IsNull())
            context->SetLocation(ais_part, trsf);
        updateLaserLine();
    }

    void setDeskModel(const TopoDS_Shape &shape) {
        if (!ais_desk.IsNull())
            context->Remove(ais_desk, Standard_False);
        ais_desk = new AIS_Shape(shape);

        Handle(Prs3d_Drawer) drawer = new Prs3d_Drawer();
        Handle(Prs3d_ShadingAspect) aShAspect = drawer->ShadingAspect();
        aShAspect->SetColor(Quantity_Color(Quantity_NOC_MATRAGRAY));
        drawer->SetShadingAspect(aShAspect);
        context->SetLocalAttributes(ais_desk, drawer, Standard_False);

        Handle(Prs3d_LineAspect) lAspect = drawer->FaceBoundaryAspect();
        lAspect->SetColor(FACE_CLR);
        drawer->SetFaceBoundaryAspect(lAspect);
        drawer->SetFaceBoundaryDraw(Standard_True);

        context->SetDisplayMode(ais_desk, bShading ? AIS_Shaded : AIS_WireFrame, Standard_False);
        context->Display(ais_desk, Standard_False);
    }

    void setDeskMdlTransform(const gp_Trsf &trsf) {
        if (!ais_desk.IsNull())
            context->SetLocation(ais_desk, trsf);
        updateLaserLine();
    }

    void setLsrheadModel(const TopoDS_Shape &shape) {
        if (!ais_lsrhead.IsNull())
            context->Remove(ais_lsrhead, Standard_False);
        ais_lsrhead = new AIS_Shape(shape);

        Handle(Prs3d_Drawer) drawer = new Prs3d_Drawer();
        Handle(Prs3d_ShadingAspect) aShAspect = drawer->ShadingAspect();
        aShAspect->SetColor(Quantity_Color(Quantity_NOC_STEELBLUE3));
        drawer->SetShadingAspect(aShAspect);
        context->SetLocalAttributes(ais_lsrhead, drawer, Standard_False);

        Handle(Prs3d_LineAspect) lAspect = drawer->FaceBoundaryAspect();
        lAspect->SetColor(FACE_CLR);
        drawer->SetFaceBoundaryAspect(lAspect);
        drawer->SetFaceBoundaryDraw(Standard_True);

        context->SetDisplayMode(ais_lsrhead, bShading ? AIS_Shaded : AIS_WireFrame, Standard_False);
        context->Display(ais_lsrhead, Standard_False);
    }

    void setLsrheadMdlTransform(const gp_Trsf &trsf) {
        if (!ais_part.IsNull())
            context->SetLocation(ais_lsrhead, trsf);

        if (!ais_laser.IsNull()) {
            context->SetLocation(ais_laser, trsf);
            updateLaserLine();
        }
    }

    void setLaserLine(const gp_Pnt &pnt, const gp_Dir &dir,
                      const double lenght, const bool clipping) {
        if (!ais_laser.IsNull())
            context->Remove(ais_laser, Standard_False);

        lsrClip = clipping;
        if (lenght > 0) {
            ais_laser = new CLaserVec(pnt, dir);
            ais_laser->setLenght(lenght);
            context->SetDisplayMode(ais_laser, AIS_Shaded, Standard_False);
            context->Display(ais_laser, Standard_False);
            context->Deactivate(ais_laser);
            updateLaserLine();
        }
    }

    void setGripModel(const TopoDS_Shape &shape) {
        if (!ais_grip.IsNull())
            context->Remove(ais_grip, Standard_False);
        ais_grip = new AIS_Shape(shape);

        Handle(Prs3d_Drawer) drawer = new Prs3d_Drawer();
        Handle(Prs3d_ShadingAspect) aShAspect = drawer->ShadingAspect();
        aShAspect->SetColor(Quantity_Color(Quantity_NOC_TOMATO3));
        drawer->SetShadingAspect(aShAspect);
        context->SetLocalAttributes(ais_grip, drawer, Standard_False);

        Handle(Prs3d_LineAspect) lAspect = drawer->FaceBoundaryAspect();
        lAspect->SetColor(FACE_CLR);
        drawer->SetFaceBoundaryAspect(lAspect);
        drawer->SetFaceBoundaryDraw(Standard_True);

        context->SetDisplayMode(ais_grip, bShading ? AIS_Shaded : AIS_WireFrame, Standard_False);
        context->Display(ais_grip, Standard_False);
    }

    void setGripMdlTransform(const gp_Trsf &trsf) {
        if (!ais_part.IsNull())
            context->SetLocation(ais_grip, trsf);
        updateLaserLine();
    }

    void hideAllAdditionalObjects() {
        context->Erase(calibTrihedron, Standard_False);
        context->Erase(ais_desk, Standard_False);
        context->Erase(ais_grip, Standard_False);
        context->Erase(ais_lsrhead, Standard_False);
        for(auto scpnt : calibPoints) {
            context->Erase(scpnt.pnt, Standard_False);
            context->Erase(scpnt.pntLbl, Standard_False);
        }
        for(auto stpnt : taskPoints) {
            context->Erase(stpnt.pnt, Standard_False);
            context->Erase(stpnt.pntLbl, Standard_False);
        }
        for(auto sppnt : pathPoints) {
            context->Erase(sppnt.pnt, Standard_False);
            context->Erase(sppnt.pntLbl, Standard_False);
        }
    }

    void showCalibObjects() {
        context->Display(calibTrihedron, Standard_False);
        context->Deactivate(calibTrihedron);
        context->Display(ais_desk, Standard_False);
        context->Deactivate(ais_desk);
        context->Display(ais_grip, Standard_False);
        context->Deactivate(ais_grip);
        context->Display(ais_lsrhead, Standard_False);
        context->Deactivate(ais_lsrhead);
        for(auto scpnt : calibPoints) {
            context->Display(scpnt.pnt, Standard_False);
            context->Display(scpnt.pntLbl, Standard_False);
            context->Deactivate(scpnt.pntLbl);
        }
    }

    void showTaskObjects() {
        context->Display(ais_desk, Standard_False);
        context->Display(ais_grip, Standard_False);
        context->Display(ais_lsrhead, Standard_False);
        for(auto stpnt : taskPoints) {
            context->Display(stpnt.pnt, Standard_False);
            context->Display(stpnt.pntLbl, Standard_False);
            context->Deactivate(stpnt.pntLbl);
        }
        for(auto sppnt : pathPoints) {
            context->Display(sppnt.pnt, Standard_False);
            context->Display(sppnt.pntLbl, Standard_False);
            context->Deactivate(sppnt.pntLbl);
        }
    }

    Handle(AIS_InteractiveObject) curShape() const {
        Handle(AIS_InteractiveObject) res = nullptr;
        const Handle(SelectMgr_EntityOwner) detectedOwner = context->DetectedOwner();
        if(!detectedOwner.IsNull()) {
            Handle(SelectMgr_SelectableObject) selected = detectedOwner->Selectable();
            if (!selected.IsNull())
                res = Handle(AIS_InteractiveObject)::DownCast(selected);
        }
        return res;
    }

    GUI_TYPES::SCalibPoint getCalibPoint(const size_t index) const {
        assert(index < calibPoints.size());
        const SCalibPoint &scpnt = calibPoints[index];
        GUI_TYPES::SCalibPoint res;
        const gp_Pnt global = scpnt.pnt->Component()->Pnt();
        res.globalPos.x = global.X();
        res.globalPos.y = global.Y();
        res.globalPos.z = global.Z();
        res.botPos = scpnt.botPos;
        return res;
    }

    GUI_TYPES::SCalibPoint getCalibLocalPoint(const size_t index) const {
        assert(index < calibPoints.size());
        const SCalibPoint &scpnt = calibPoints[index];
        GUI_TYPES::SCalibPoint res;
        res.botPos = scpnt.botPos;
        const gp_Trsf partTr = context->Location(ais_part).Transformation();
        const gp_Pnt local = scpnt.pnt->Component()->Pnt().Transformed(partTr.Inverted());
        res.globalPos.x = local.X();
        res.globalPos.y = local.Y();
        res.globalPos.z = local.Z();
        return res;
    }

    void appendCalibPoint(const GUI_TYPES::SCalibPoint &calibPoint) {
        SCalibPoint scpnt;
        scpnt.botPos = calibPoint.botPos;
        const gp_Pnt globalPos(calibPoint.globalPos.x, calibPoint.globalPos.y, calibPoint.globalPos.z);
        scpnt.pnt = new AIS_Point(new Geom_CartesianPoint(globalPos));
        scpnt.pntLbl = new AIS_TextLabel();
        scpnt.pntLbl->SetPosition(globalPos);
        std::stringstream ss;
        ss << "C" << calibPoints.size() + 1;
        scpnt.pntLbl->SetText(TCollection_ExtendedString(ss.str().c_str(), Standard_True));
        calibPoints.push_back(scpnt);
        context->Display(scpnt.pnt, Standard_False);
        context->SetZLayer(scpnt.pntLbl, depthTestOffZlayer);
        context->Display(scpnt.pntLbl, Standard_False);
        context->Deactivate(scpnt.pntLbl);
    }

    void changeCalibPoint(const size_t index, const GUI_TYPES::SCalibPoint &calibPoint) {
        assert(index < calibPoints.size());
        SCalibPoint &scpnt = calibPoints[index];
        scpnt.botPos = calibPoint.botPos;
        const gp_Pnt globalPos(calibPoint.globalPos.x, calibPoint.globalPos.y, calibPoint.globalPos.z);
        scpnt.pnt->SetComponent(new Geom_CartesianPoint(globalPos));
        scpnt.pntLbl->SetPosition(globalPos);
        context->RecomputePrsOnly(scpnt.pnt, Standard_False);
        context->RecomputePrsOnly(scpnt.pntLbl, Standard_False);
    }

    void removeCalibPoint(const size_t index) {
        assert(index < calibPoints.size());
        SCalibPoint &scpnt = calibPoints[index];
        context->Remove(scpnt.pnt, Standard_False);
        context->Remove(scpnt.pntLbl, Standard_False);
        calibPoints.erase(calibPoints.cbegin() + index);
        for(size_t i = 0; i < calibPoints.size(); ++i) {
            std::stringstream ss;
            ss << "C" << i + 1;
            calibPoints[i].pntLbl->SetText(TCollection_ExtendedString(ss.str().c_str(), Standard_True));
            context->RecomputePrsOnly(calibPoints[i].pntLbl, Standard_False);
        }
    }

    GUI_TYPES::STaskPoint getTaskPoint(const size_t index) const {
        assert(index < taskPoints.size());
        const STaskPoint &stpnt = taskPoints[index];
        GUI_TYPES::STaskPoint res;
        res.taskType = stpnt.taskType;
        const gp_Pnt global = stpnt.pnt->Component()->Pnt();
        res.globalPos.x = global.X();
        res.globalPos.y = global.Y();
        res.globalPos.z = global.Z();
        res.angle = stpnt.angle;
        return res;
    }

    std::string taskPointName(const size_t index, const GUI_TYPES::TBotTaskType taskType) const {
        using namespace GUI_TYPES;
        const std::map <GUI_TYPES::TBotTaskType, std::string> mapNames = {
            { ENBTT_MOVE , "Перемещение" },
            { ENBTT_DRILL, "Сверление"   },
            { ENBTT_MARK , "Маркировка"  }
        };
        std::stringstream ss;
        ss << "T" << index + 1<< " (" << extract_map_value(mapNames, taskType, std::string(" ")) << ")";
        return ss.str();
    }

    void appendTaskPoint(const GUI_TYPES::STaskPoint &taskPoint) {
        STaskPoint stpnt;
        stpnt.taskType = taskPoint.taskType;
        stpnt.angle = taskPoint.angle;
        const gp_Pnt globalPos(taskPoint.globalPos.x, taskPoint.globalPos.y, taskPoint.globalPos.z);
        stpnt.pnt = new AIS_Point(new Geom_CartesianPoint(globalPos));
        stpnt.pntLbl = new AIS_TextLabel();
        stpnt.pntLbl->SetPosition(globalPos);
        const std::string txt = taskPointName(taskPoints.size(), stpnt.taskType);
        stpnt.pntLbl->SetText(TCollection_ExtendedString(txt.c_str(), Standard_True));
        gp_Dir zDir(taskPoint.normal.x, taskPoint.normal.y, taskPoint.normal.z);
        stpnt.tPnt = new CTaskPnt(globalPos, zDir, 5.);
        taskPoints.push_back(stpnt);
        context->Display(stpnt.pnt, Standard_False);
        context->SetZLayer(stpnt.pntLbl, depthTestOffZlayer);
        context->Display(stpnt.pntLbl, Standard_False);
        context->Deactivate(stpnt.pntLbl);
        context->Display(stpnt.tPnt, Standard_False);
        context->Deactivate(stpnt.tPnt);
    }

    void changeTaskPoint(const size_t index, const GUI_TYPES::STaskPoint &taskPoint) {
        assert(index < taskPoints.size());
        STaskPoint &stpnt = taskPoints[index];
        stpnt.angle = taskPoint.angle;
        const gp_Pnt globalPos(taskPoint.globalPos.x, taskPoint.globalPos.y, taskPoint.globalPos.z);
        stpnt.pnt->SetComponent(new Geom_CartesianPoint(globalPos));
        stpnt.pntLbl->SetPosition(globalPos);
        gp_Dir zDir(taskPoint.normal.x, taskPoint.normal.y, taskPoint.normal.z);
        stpnt.tPnt = new CTaskPnt(globalPos, zDir, 5.);
        context->RecomputePrsOnly(stpnt.pnt, Standard_False);
        context->RecomputePrsOnly(stpnt.pntLbl, Standard_False);
        context->Display(stpnt.tPnt, Standard_False);
        context->Deactivate(stpnt.tPnt);
    }

    void removeTaskPoint(const size_t index) {
        assert(index < taskPoints.size());
        STaskPoint &stpnt = taskPoints[index];
        context->Remove(stpnt.pnt, Standard_False);
        context->Remove(stpnt.pntLbl, Standard_False);
        taskPoints.erase(taskPoints.cbegin() + index);
        for(size_t i = 0; i < taskPoints.size(); ++i)
        {
            const std::string txt = taskPointName(i, taskPoints[i].taskType);
            taskPoints[i].pntLbl->SetText(TCollection_ExtendedString(txt.c_str(), Standard_True));
            context->RecomputePrsOnly(taskPoints[i].pntLbl, Standard_False);
        }
    }

    GUI_TYPES::SPathPoint getPathPoint(const size_t index) const {
        assert(index < pathPoints.size());
        const SPathPoint &sppnt = pathPoints[index];
        GUI_TYPES::SPathPoint res;
        res.angle = sppnt.angle;
        const gp_Pnt global = sppnt.pnt->Component()->Pnt();
        res.globalPos.x = global.X();
        res.globalPos.y = global.Y();
        res.globalPos.z = global.Z();
        return res;
    }

    void appendPathPoint(const GUI_TYPES::SPathPoint &pathPoint) {
        SPathPoint sppnt;
        sppnt.angle = pathPoint.angle;
        const gp_Pnt globalPos(pathPoint.globalPos.x, pathPoint.globalPos.y, pathPoint.globalPos.z);
        sppnt.pnt = new AIS_Point(new Geom_CartesianPoint(globalPos));
        sppnt.pntLbl = new AIS_TextLabel();
        sppnt.pntLbl->SetPosition(globalPos);
        std::stringstream ss;
        ss << "P" << pathPoints.size() + 1;
        sppnt.pntLbl->SetText(TCollection_ExtendedString(ss.str().c_str(), Standard_True));
        pathPoints.push_back(sppnt);
        context->Display(sppnt.pnt, Standard_False);
        context->SetZLayer(sppnt.pntLbl, depthTestOffZlayer);
        context->Display(sppnt.pntLbl, Standard_False);
        context->Deactivate(sppnt.pntLbl);
    }

    void changePathPoint(const size_t index, const GUI_TYPES::SPathPoint &pathPoint) {
        assert(index < pathPoints.size());
        SPathPoint &sppnt = pathPoints[index];
        sppnt.angle = pathPoint.angle;
        const gp_Pnt globalPos(pathPoint.globalPos.x, pathPoint.globalPos.y, pathPoint.globalPos.z);
        sppnt.pnt->SetComponent(new Geom_CartesianPoint(globalPos));
        sppnt.pntLbl->SetPosition(globalPos);
        context->RecomputePrsOnly(sppnt.pnt, Standard_False);
        context->RecomputePrsOnly(sppnt.pntLbl, Standard_False);
    }

    void removePathPoint(const size_t index) {
        assert(index < pathPoints.size());
        SPathPoint &sppnt = pathPoints[index];
        context->Remove(sppnt.pnt, Standard_False);
        context->Remove(sppnt.pntLbl, Standard_False);
        pathPoints.erase(pathPoints.cbegin() + index);
        for(size_t i = 0; i < pathPoints.size(); ++i) {
            std::stringstream ss;
            ss << "P" << i + 1;
            pathPoints[i].pntLbl->SetText(TCollection_ExtendedString(ss.str().c_str(), Standard_True));
            context->RecomputePrsOnly(pathPoints[i].pntLbl, Standard_False);
        }
    }

    bool detectNormal(gp_Dir &normal, const gp_Pnt pnt, const Handle(AIS_Shape) &obj) const {
        normal = gp_Dir(0., 0., 1.);
        const gp_Trsf partTr = context->Location(obj).Transformation();
        const gp_Pnt localCoord = pnt.Transformed(partTr.Inverted());
        for (TopExp_Explorer anExp(obj->Shape(), TopAbs_FACE); anExp.More(); anExp.Next()) {
            const TopoDS_Face face = TopoDS::Face(anExp.Current());
            BRepClass3d_SolidClassifier classifier(face);
            classifier.Perform(localCoord, Precision::Intersection());
            if (classifier.State() == TopAbs_ON) {
                Standard_Real umin, umax, vmin, vmax;
                BRepTools::UVBounds(face, umin, umax, vmin, vmax);
                Handle(Geom_Surface) aSurface = BRep_Tool::Surface(face);
                GeomLProp_SLProps props(aSurface, umin, vmin,1, 0.01);
                normal = props.Normal();
                if(face.Orientation() == TopAbs_REVERSED)
                    normal.Reverse();
                normal.Transform(partTr);
                return true;
            }
        }
        return false;
    }

    void updateLaserLine() {
        if (lsrClip && !ais_laser.IsNull()) {
            NCollection_Vector <Handle(AIS_Shape)> vecObj;
            vecObj.Append(ais_part);
            vecObj.Append(ais_grip);
            vecObj.Append(ais_desk);
            ais_laser->clipLenght(context, vecObj);
            context->RecomputePrsOnly(ais_laser, Standard_False);
        }
    }

private:
    Graphic3d_ZLayerId depthTestOffZlayer;
    bool bShading;
    bool bGripVisible;

    //AIS_Objects
    Handle(AIS_InteractiveContext) context;

    Handle(AIS_ViewCube) ais_axis_cube;
    Handle(AIS_Shape) ais_part;
    Handle(AIS_Shape) ais_desk;
    Handle(AIS_Shape) ais_lsrhead;
    Handle(AIS_Shape) ais_grip;
    Handle(CLaserVec) ais_laser;
    bool lsrClip;

    Handle(AIS_Trihedron) calibTrihedron;
    bool bCursorIsVisible;
    Handle(AIS_Point) cursorPnt;
    Handle(AIS_TextLabel) cursorLbl;

    struct SCalibPoint
    {
        GUI_TYPES::SVertex botPos;
        Handle(AIS_Point) pnt;
        Handle(AIS_TextLabel) pntLbl;
    };
    std::vector <SCalibPoint> calibPoints;

    struct STaskPoint
    {
        GUI_TYPES::TBotTaskType taskType;
        GUI_TYPES::SRotationAngle angle;
        Handle(AIS_Point) pnt;
        Handle(AIS_TextLabel) pntLbl;
        Handle(CTaskPnt) tPnt;
    };
    std::vector <STaskPoint> taskPoints;

    struct SPathPoint
    {
        GUI_TYPES::SRotationAngle angle;
        Handle(AIS_Point) pnt;
        Handle(AIS_TextLabel) pntLbl;
    };
    std::vector <SPathPoint> pathPoints;
};



CInteractiveContext::CInteractiveContext() :
    d_ptr(new CInteractiveContextPrivate())
{

}

CInteractiveContext::~CInteractiveContext()
{
    delete d_ptr;
}

AIS_InteractiveContext &CInteractiveContext::context()
{
    return *d_ptr->context;
}

void CInteractiveContext::setDisableTepthTestZLayer(const Graphic3d_ZLayerId zLayerWithoutDepthTest)
{
    d_ptr->depthTestOffZlayer = zLayerWithoutDepthTest;
}

void CInteractiveContext::init(AIS_InteractiveContext &context)
{
    d_ptr->init(context);
}

void CInteractiveContext::setShading(const bool enabled)
{
    d_ptr->setShading(enabled);
}

void CInteractiveContext::updateCursorPosition()
{
    d_ptr->updateCursorPosition();
}

void CInteractiveContext::resetCursorPosition()
{
    d_ptr->resetCursorPosition();
}

gp_Pnt CInteractiveContext::lastCursorPosition() const
{
    const gp_Trsf transform =
            d_ptr->context->Location(d_ptr->cursorPnt).Transformation();
    return d_ptr->cursorPnt->Component()->Pnt().Transformed(transform);
}

void CInteractiveContext::setPartModel(const TopoDS_Shape &shape)
{
    d_ptr->setPartModel(shape);
}

void CInteractiveContext::setPartMdlTransform(const gp_Trsf &trsf)
{
    d_ptr->setPartMdlTransform(trsf);
}

void CInteractiveContext::setDeskModel(const TopoDS_Shape &shape)
{
    d_ptr->setDeskModel(shape);
}

void CInteractiveContext::setDeskMdlTransform(const gp_Trsf &trsf)
{
    d_ptr->setDeskMdlTransform(trsf);
}

void CInteractiveContext::setLsrheadModel(const TopoDS_Shape &shape)
{
    d_ptr->setLsrheadModel(shape);
}

void CInteractiveContext::setLsrheadMdlTransform(const gp_Trsf &trsf)
{
    d_ptr->setLsrheadMdlTransform(trsf);
}

void CInteractiveContext::setLaserLine(const gp_Pnt &pnt, const gp_Dir &dir,
                                       const double lenght, const bool clipping)
{
    d_ptr->setLaserLine(pnt, dir, lenght, clipping);
}

void CInteractiveContext::setGripModel(const TopoDS_Shape &shape)
{
    d_ptr->setGripModel(shape);
}

void CInteractiveContext::setGripMdlTransform(const gp_Trsf &trsf)
{
    d_ptr->setGripMdlTransform(trsf);
}

const TopoDS_Shape &CInteractiveContext::getPartShape() const
{
    return d_ptr->ais_part->Shape();
}

const gp_Trsf &CInteractiveContext::getPartTransform() const
{
    return d_ptr->context->Location(d_ptr->ais_part).Transformation();
}

const TopoDS_Shape &CInteractiveContext::getDeskShape() const
{
    return d_ptr->ais_desk->Shape();
}

const TopoDS_Shape &CInteractiveContext::getLsrHeadShape() const
{
    return d_ptr->ais_lsrhead->Shape();
}

const TopoDS_Shape &CInteractiveContext::getGripShape() const
{
    return d_ptr->ais_grip->Shape();
}

void CInteractiveContext::hideAllAdditionalObjects()
{
    d_ptr->hideAllAdditionalObjects();
}

void CInteractiveContext::showCalibObjects()
{
    d_ptr->showCalibObjects();
}

void CInteractiveContext::showTaskObjects()
{
    d_ptr->showTaskObjects();
}

void CInteractiveContext::setGripVisible(const bool enabled, const bool selectable)
{
    if (enabled)
    {
        d_ptr->context->Display(d_ptr->ais_grip, Standard_False);
        if (!selectable)
            d_ptr->context->Deactivate(d_ptr->ais_grip);
    }
    else
        d_ptr->context->Erase(d_ptr->ais_grip, Standard_False);
}

bool CInteractiveContext::isDeskDetected() const
{
    return d_ptr->curShape() == d_ptr->ais_desk;
}

bool CInteractiveContext::isPartDetected() const
{
    return d_ptr->curShape() == d_ptr->ais_part;
}

bool CInteractiveContext::isGripDetected() const
{
    return d_ptr->curShape() == d_ptr->ais_grip;
}

bool CInteractiveContext::isLsrheadDetected() const
{
    return d_ptr->curShape() == d_ptr->ais_lsrhead;
}

bool CInteractiveContext::isCalibPointDetected(size_t &index) const
{
    Handle(AIS_InteractiveObject) curShape = d_ptr->curShape();
    index = 0;
    for (auto scpnt : d_ptr->calibPoints)
    {
        if (scpnt.pnt == curShape)
            return true;

        ++index;
    }
    return false;
}

bool CInteractiveContext::isTaskPointDetected(size_t &index) const
{
    Handle(AIS_InteractiveObject) curShape = d_ptr->curShape();
    index = 0;
    for (auto stpnt : d_ptr->taskPoints)
    {
        if (stpnt.pnt == curShape)
            return true;
        ++index;
    }
    return false;
}

bool CInteractiveContext::isPathPointDetected(size_t &index) const
{
    Handle(AIS_InteractiveObject) curShape = d_ptr->curShape();
    index = 0;
    for (auto sppnt : d_ptr->pathPoints)
    {
        if (sppnt.pnt == curShape)
            return true;
        ++index;
    }
    return false;
}

size_t CInteractiveContext::getCalibPointCount() const
{
    return d_ptr->calibPoints.size();
}

GUI_TYPES::SCalibPoint CInteractiveContext::getCalibPoint(const size_t index) const
{
    return d_ptr->getCalibPoint(index);
}

GUI_TYPES::SCalibPoint CInteractiveContext::getCalibLocalPoint(const size_t index) const
{
    return d_ptr->getCalibLocalPoint(index);
}

void CInteractiveContext::appendCalibPoint(const GUI_TYPES::SCalibPoint &calibPoint)
{
    d_ptr->appendCalibPoint(calibPoint);
}

void CInteractiveContext::changeCalibPoint(const size_t index, const GUI_TYPES::SCalibPoint &calibPoint)
{
    d_ptr->changeCalibPoint(index, calibPoint);
}

void CInteractiveContext::removeCalibPoint(const size_t index)
{
    d_ptr->removeCalibPoint(index);
}

size_t CInteractiveContext::getTaskPointCount() const
{
    return d_ptr->taskPoints.size();
}

GUI_TYPES::STaskPoint CInteractiveContext::getTaskPoint(const size_t index) const
{
    return d_ptr->getTaskPoint(index);
}

void CInteractiveContext::appendTaskPoint(const GUI_TYPES::STaskPoint &taskPoint)
{
    d_ptr->appendTaskPoint(taskPoint);
}

void CInteractiveContext::changeTaskPoint(const size_t index, const GUI_TYPES::STaskPoint &taskPoint)
{
    d_ptr->changeTaskPoint(index, taskPoint);
}

void CInteractiveContext::removeTaskPoint(const size_t index)
{
    d_ptr->removeTaskPoint(index);
}

size_t CInteractiveContext::getPathPointCount() const
{
    return d_ptr->pathPoints.size();
}

GUI_TYPES::SPathPoint CInteractiveContext::getPathPoint(const size_t index) const
{
    return d_ptr->getPathPoint(index);
}

void CInteractiveContext::appendPathPoint(const GUI_TYPES::SPathPoint &pathPoint)
{
    d_ptr->appendPathPoint(pathPoint);
}

void CInteractiveContext::changePathPoint(const size_t index, const GUI_TYPES::SPathPoint &pathPoint)
{
    d_ptr->changePathPoint(index, pathPoint);
}

void CInteractiveContext::removePathPoint(const size_t index)
{
    d_ptr->removePathPoint(index);
}

gp_Dir CInteractiveContext::detectNormal(const gp_Pnt pnt) const
{
    gp_Dir normal(0., 0., 1.);
    bool res = d_ptr->detectNormal(normal, pnt, d_ptr->ais_part);
    if (!res)
        d_ptr->detectNormal(normal, pnt, d_ptr->ais_desk);
    return normal;
}
