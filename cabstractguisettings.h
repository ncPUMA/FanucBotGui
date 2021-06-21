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
};


class CEmptyGuiSettings : public CAbstractGuiSettings
{
    GUI_TYPES::TMSAA getMsaa() const override { return 0; }
    void setMsaa(const GUI_TYPES::TMSAA) { }

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
};

#endif // CABSTRACTGUISETTINGS_H
