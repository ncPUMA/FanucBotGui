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

    SGuiSettings loadGuiSettings();
    void saveGuiSettings(const SGuiSettings &settings);

private:
    CSimpleSettingsStoragePrivate * const d_ptr;
};

#endif // CSIMPLESETTINGSSTORAGE_H
