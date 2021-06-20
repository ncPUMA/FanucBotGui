#ifndef CABSTRACTGUISETTINGS_H
#define CABSTRACTGUISETTINGS_H

#include "gui_types.h"

class CAbstractGuiSettings
{
public:
    CAbstractGuiSettings() { }
    virtual ~CAbstractGuiSettings() { }

    virtual GUI_TYPES::TMSAA getMsaa() const = 0;
    virtual void setMsaa(const GUI_TYPES::TMSAA value) = 0;

    //The LaserBot
    virtual double getBotAnchorX() const = 0;
    virtual double getBotAnchorY() const = 0;
    virtual double getBotAnchorZ() const = 0;
    virtual double getBotLaserX() const = 0;
    virtual double getBotLaserY() const = 0;
    virtual double getBotLaserZ() const = 0;
    virtual GUI_TYPES::TCoordSystem getBotCoordType() const = 0;

    virtual void setBotAnchorX(const double value) = 0;
    virtual void setBotAnchorY(const double value) = 0;
    virtual void setBotAnchorZ(const double value) = 0;
    virtual void setBotLaserX(const double value) = 0;
    virtual void setBotLaserY(const double value) = 0;
    virtual void setBotLaserZ(const double value) = 0;
    virtual void setBotCoordType(const GUI_TYPES::TCoordSystem value) = 0;

    //The Part
    virtual double getPartTrX() const = 0;
    virtual double getPartTrY() const = 0;
    virtual double getPartTrZ() const = 0;
    virtual double getPartCenterX() const = 0;
    virtual double getPartCenterY() const = 0;
    virtual double getPartCenterZ() const = 0;
    virtual double getPartRotationX() const = 0;
    virtual double getPartRotationY() const = 0;
    virtual double getPartRotationZ() const = 0;
    virtual double getPartScale() const = 0;

    virtual void setPartTrX(const double value) = 0;
    virtual void setPartTrY(const double value) = 0;
    virtual void setPartTrZ(const double value) = 0;
    virtual void setPartCenterX(const double value) = 0;
    virtual void setPartCenterY(const double value) = 0;
    virtual void setPartCenterZ(const double value) = 0;
    virtual void setPartRotationX(const double value) = 0;
    virtual void setPartRotationY(const double value) = 0;
    virtual void setPartRotationZ(const double value) = 0;
    virtual void setPartScale(const double value) = 0;

    //The Grip
    virtual bool   isGripVisible() const = 0;
    virtual double getGripTrX() const = 0;
    virtual double getGripTrY() const = 0;
    virtual double getGripTrZ() const = 0;
    virtual double getGripCenterX() const = 0;
    virtual double getGripCenterY() const = 0;
    virtual double getGripCenterZ() const = 0;
    virtual double getGripRotationX() const = 0;
    virtual double getGripRotationY() const = 0;
    virtual double getGripRotationZ() const = 0;
    virtual double getGripScale() const = 0;

    virtual void setGripVisible(const bool value) = 0;
    virtual void setGripTrX(const double value) = 0;
    virtual void setGripTrY(const double value) = 0;
    virtual void setGripTrZ(const double value) = 0;
    virtual void setGripCenterX(const double value) = 0;
    virtual void setGripCenterY(const double value) = 0;
    virtual void setGripCenterZ(const double value) = 0;
    virtual void setGripRotationX(const double value) = 0;
    virtual void setGripRotationY(const double value) = 0;
    virtual void setGripRotationZ(const double value) = 0;
    virtual void setGripScale(const double value) = 0;
};


class CEmptyGuiSettings : public CAbstractGuiSettings
{
    GUI_TYPES::TMSAA getMsaa() const override { return 0; }
    void setMsaa(const GUI_TYPES::TMSAA) { }

    //The LaserBot
    double getBotAnchorX() const override { return 0; }
    double getBotAnchorY() const override { return 0; }
    double getBotAnchorZ() const  override { return 0; }
    double getBotLaserX() const override { return 0; }
    double getBotLaserY() const override { return 0; }
    double getBotLaserZ() const  override { return 0; }
    GUI_TYPES::TCoordSystem getBotCoordType() const override { return 0; }

    void setBotAnchorX(const double) override { }
    void setBotAnchorY(const double) override { }
    void setBotAnchorZ(const double) override { }
    void setBotLaserX(const double) override { }
    void setBotLaserY(const double) override { }
    void setBotLaserZ(const double) override { }
    void setBotCoordType(const GUI_TYPES::TCoordSystem) override { }

    //The Part
    double getPartTrX() const override { return 0; }
    double getPartTrY() const override { return 0; }
    double getPartTrZ() const override { return 0; }
    double getPartCenterX() const override { return 0; }
    double getPartCenterY() const override { return 0; }
    double getPartCenterZ() const override { return 0; }
    double getPartRotationX() const override { return 0; }
    double getPartRotationY() const override { return 0; }
    double getPartRotationZ() const override { return 0; }
    double getPartScale() const override { return 0; }

    void setPartTrX(const double) override { }
    void setPartTrY(const double) override { }
    void setPartTrZ(const double) override { }
    void setPartCenterX(const double) override { }
    void setPartCenterY(const double) override { }
    void setPartCenterZ(const double) override { }
    void setPartRotationX(const double) override { }
    void setPartRotationY(const double) override { }
    void setPartRotationZ(const double) override { }
    void setPartScale(const double) override { }

    //The Grip
    bool   isGripVisible() const  override { return false; }
    double getGripTrX() const override { return 0; }
    double getGripTrY() const override { return 0; }
    double getGripTrZ() const override { return 0; }
    double getGripCenterX() const override { return 0; }
    double getGripCenterY() const override { return 0; }
    double getGripCenterZ() const override { return 0; }
    double getGripRotationX() const override { return 0; }
    double getGripRotationY() const override { return 0; }
    double getGripRotationZ() const override { return 0; }
    double getGripScale() const override { return 0; }

    void setGripVisible(const bool) override { }
    void setGripTrX(const double) override { }
    void setGripTrY(const double) override { }
    void setGripTrZ(const double) override { }
    void setGripCenterX(const double) override { }
    void setGripCenterY(const double) override { }
    void setGripCenterZ(const double) override { }
    void setGripRotationX(const double) override { }
    void setGripRotationY(const double) override { }
    void setGripRotationZ(const double) override { }
    void setGripScale(const double) override { }
};

#endif // CABSTRACTGUISETTINGS_H
