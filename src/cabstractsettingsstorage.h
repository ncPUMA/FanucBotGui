#ifndef CABSTRACTSETTINGSSTORAGE_H
#define CABSTRACTSETTINGSSTORAGE_H

#include "sguisettings.h"

class CAbstractSettingsStorage
{
public:
    virtual ~CAbstractSettingsStorage() { }

    virtual SGuiSettings loadGuiSettings() = 0;
    virtual void saveGuiSettings(const SGuiSettings &settings) = 0;

protected:
    CAbstractSettingsStorage() { }
};


class CEmptySettingsStorage : public CAbstractSettingsStorage
{
public:
    CEmptySettingsStorage() : CAbstractSettingsStorage() { }

    SGuiSettings loadGuiSettings() { return SGuiSettings(); }
    void saveGuiSettings(const SGuiSettings &) { }
};

#endif // CABSTRACTSETTINGSSTORAGE_H
