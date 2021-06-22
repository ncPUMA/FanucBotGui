#include "csimplesettingsstorage.h"

#include <QSettings>

static const char *GUI_PREFIX = "GUI";

enum EN_GuiKeys
{
    ENGK_FIRST = 0,

    ENGK_MSAA = ENGK_FIRST,

    //The Part
    ENGK_PART_TR_X,
    ENGK_PART_TR_Y,
    ENGK_PART_TR_Z,
    ENGK_PART_CENTER_X,
    ENGK_PART_CENTER_Y,
    ENGK_PART_CENTER_Z,
    ENGK_PART_ROTATE_X,
    ENGK_PART_ROTATE_Y,
    ENGK_PART_ROTATE_Z,
    ENGK_PART_SCALE,

    ENGK_LAST
};
typedef int TGuiKey;

static const std::map <TGuiKey, QString> guiKeyMap = {
    { ENGK_MSAA          , "msaa"           },
    //The Part
    { ENGK_PART_TR_X     , "part_tr_x"      },
    { ENGK_PART_TR_Y     , "part_tr_y"      },
    { ENGK_PART_TR_Z     , "part_tr_z"      },
    { ENGK_PART_CENTER_X , "part_center_x"  },
    { ENGK_PART_CENTER_Y , "part_center_y"  },
    { ENGK_PART_CENTER_Z , "part_center_z"  },
    { ENGK_PART_ROTATE_X , "part_rotate_x"  },
    { ENGK_PART_ROTATE_Y , "part_rotate_y"  },
    { ENGK_PART_ROTATE_Z , "part_rotate_z"  },
    { ENGK_PART_SCALE    , "part_scale"     }
};

class CSimpleSettingsStoragePrivate
{
public:
    CSimpleSettingsStoragePrivate() :
        settingsFile(nullptr)
    { }

    ~CSimpleSettingsStoragePrivate() {
        delete settingsFile;
    }

    void setFName(const char *fname) {
        delete settingsFile;
        settingsFile = new QSettings(QString::fromLocal8Bit(fname), QSettings::IniFormat);
    }

    SGuiSettings loadGuiSettings() {
        SGuiSettings res;
        settingsFile->beginGroup(GUI_PREFIX);
        res.partTrX       = readGuiValue <double> (ENGK_PART_TR_X);
        res.partTrY       = readGuiValue <double> (ENGK_PART_TR_Y);
        res.partTrZ       = readGuiValue <double> (ENGK_PART_TR_Z);
        res.partCenterX   = readGuiValue <double> (ENGK_PART_CENTER_X);
        res.partCenterY   = readGuiValue <double> (ENGK_PART_CENTER_Y);
        res.partCenterZ   = readGuiValue <double> (ENGK_PART_CENTER_Z);
        res.partRotationX = readGuiValue <double> (ENGK_PART_ROTATE_X);
        res.partRotationY = readGuiValue <double> (ENGK_PART_ROTATE_Y);
        res.partRotationZ = readGuiValue <double> (ENGK_PART_ROTATE_Z);
        res.partScale     = readGuiValue <double> (ENGK_PART_SCALE);
        settingsFile->endGroup();
        return res;
    }

    void saveGuiSettings(const SGuiSettings &settings) {
        settingsFile->beginGroup(GUI_PREFIX);
        writeGuiValue(ENGK_PART_TR_X    , settings.partTrX);
        writeGuiValue(ENGK_PART_TR_Y    , settings.partTrY);
        writeGuiValue(ENGK_PART_TR_Z    , settings.partTrZ);
        writeGuiValue(ENGK_PART_CENTER_X, settings.partCenterX);
        writeGuiValue(ENGK_PART_CENTER_Y, settings.partCenterY);
        writeGuiValue(ENGK_PART_CENTER_Z, settings.partCenterZ);
        writeGuiValue(ENGK_PART_ROTATE_X, settings.partRotationX);
        writeGuiValue(ENGK_PART_ROTATE_Y, settings.partRotationY);
        writeGuiValue(ENGK_PART_ROTATE_Z, settings.partRotationZ);
        writeGuiValue(ENGK_PART_SCALE   , settings.partScale);
        settingsFile->endGroup();
    }

private:
    template <typename T>
    T readGuiValue(const TGuiKey key) const {
        T result = T();
        auto it = guiKeyMap.find(key);
        if (it != guiKeyMap.cend()) {
            result = settingsFile->value(it->second).value <T> ();
        }
        return result;
    }

    template <typename T>
    void writeGuiValue(const TGuiKey key, const T value) {
        auto it = guiKeyMap.find(key);
        if (it != guiKeyMap.cend()) {
            settingsFile->setValue(it->second, QVariant::fromValue <T> (value));
        }
    }

private:
    QSettings *settingsFile;
};

CSimpleSettingsStorage::CSimpleSettingsStorage() :
    CAbstractSettingsStorage(),
    d_ptr(new CSimpleSettingsStoragePrivate())
{

}

CSimpleSettingsStorage::~CSimpleSettingsStorage()
{
    delete d_ptr;
}

void CSimpleSettingsStorage::setSettingsFName(const char *fname)
{
    d_ptr->setFName(fname);
}

SGuiSettings CSimpleSettingsStorage::loadGuiSettings()
{
    return d_ptr->loadGuiSettings();
}

void CSimpleSettingsStorage::saveGuiSettings(const SGuiSettings &settings)
{
    d_ptr->saveGuiSettings(settings);
}
