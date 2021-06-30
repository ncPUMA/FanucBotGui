#ifndef CABSTRACTSETTINGSSTORAGE_H
#define CABSTRACTSETTINGSSTORAGE_H

#include "sguisettings.h"

class CAbstractSettingsStorage
{
public:
    virtual ~CAbstractSettingsStorage() { }

    virtual GUI_TYPES::SGuiSettings loadGuiSettings() = 0;
    virtual void saveGuiSettings(const GUI_TYPES::SGuiSettings &settings) = 0;

protected:
    CAbstractSettingsStorage() { }
};


class CEmptySettingsStorage : public CAbstractSettingsStorage
{
public:
    CEmptySettingsStorage() : CAbstractSettingsStorage() { }

    GUI_TYPES::SGuiSettings loadGuiSettings() { return GUI_TYPES::SGuiSettings(); }
    void saveGuiSettings(const GUI_TYPES::SGuiSettings &) { }
};

#endif // CABSTRACTSETTINGSSTORAGE_H
