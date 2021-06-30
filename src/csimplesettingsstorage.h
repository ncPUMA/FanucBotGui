#ifndef CSIMPLESETTINGSSTORAGE_H
#define CSIMPLESETTINGSSTORAGE_H

#include "cabstractsettingsstorage.h"

class CSimpleSettingsStoragePrivate;

class CSimpleSettingsStorage : public CAbstractSettingsStorage
{
public:
    CSimpleSettingsStorage();
    ~CSimpleSettingsStorage();

    void setSettingsFName(const char *fname);

    GUI_TYPES::SGuiSettings loadGuiSettings();
    void saveGuiSettings(const GUI_TYPES::SGuiSettings &settings);

private:
    CSimpleSettingsStoragePrivate * const d_ptr;
};

#endif // CSIMPLESETTINGSSTORAGE_H
