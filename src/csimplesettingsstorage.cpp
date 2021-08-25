#include "csimplesettingsstorage.h"

#include <QSettings>

static const char *GUI_PREFIX = "GUI";

enum EN_GuiKeys
{
    ENGK_FIRST = 0,

    //Common
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
    //The Desk
    ENGK_DESK_TR_X,
    ENGK_DESK_TR_Y,
    ENGK_DESK_TR_Z,
    ENGK_DESK_CENTER_X,
    ENGK_DESK_CENTER_Y,
    ENGK_DESK_CENTER_Z,
    ENGK_DESK_ROTATE_X,
    ENGK_DESK_ROTATE_Y,
    ENGK_DESK_ROTATE_Z,
    ENGK_DESK_SCALE,
    //The Laser Head
    ENGK_LHEAD_TR_X,
    ENGK_LHEAD_TR_Y,
    ENGK_LHEAD_TR_Z,
    ENGK_LHEAD_CENTER_X,
    ENGK_LHEAD_CENTER_Y,
    ENGK_LHEAD_CENTER_Z,
    ENGK_LHEAD_ROTATE_X,
    ENGK_LHEAD_ROTATE_Y,
    ENGK_LHEAD_ROTATE_Z,
    ENGK_LHEAD_SCALE,
    ENGK_LHEAD_LSR_TR_X,
    ENGK_LHEAD_LSR_TR_Y,
    ENGK_LHEAD_LSR_TR_Z,
    ENGK_LHEAD_LSR_NRML_X,
    ENGK_LHEAD_LSR_NRML_Y,
    ENGK_LHEAD_LSR_NRML_Z,
    ENGK_LHEAD_LSR_LENGHT,
    ENGK_LHEAD_LSR_CLIP,
    //The Grip
    ENGK_GRIP_TR_X,
    ENGK_GRIP_TR_Y,
    ENGK_GRIP_TR_Z,
    ENGK_GRIP_CENTER_X,
    ENGK_GRIP_CENTER_Y,
    ENGK_GRIP_CENTER_Z,
    ENGK_GRIP_ROTATE_X,
    ENGK_GRIP_ROTATE_Y,
    ENGK_GRIP_ROTATE_Z,
    ENGK_GRIP_SCALE,
    ENGK_GRIP_VIS,

    //Common
    ENGK_SNAP_SCALE,
    ENGK_SNAP_WIDTH,
    ENGK_SNAP_HEIGHT,

    //Models
    ENGK_MDL_PART,
    ENGK_MDL_GRIP,
    ENGK_MDL_DESK,
    ENGK_MDL_LHEAD,

    ENGK_LAST
};
typedef int TGuiKey;

static const std::map <TGuiKey, QString> guiKeyMap = {
    { ENGK_MSAA          , "msaa"           },
    { ENGK_SNAP_SCALE    , "snap_scale"     },
    { ENGK_SNAP_WIDTH    , "snap_width"     },
    { ENGK_SNAP_HEIGHT   , "snap_height"     },
    //The Part
    { ENGK_PART_TR_X     , "part/tr_x"     },
    { ENGK_PART_TR_Y     , "part/tr_y"     },
    { ENGK_PART_TR_Z     , "part/tr_z"     },
    { ENGK_PART_CENTER_X , "part/center_x" },
    { ENGK_PART_CENTER_Y , "part/center_y" },
    { ENGK_PART_CENTER_Z , "part/center_z" },
    { ENGK_PART_ROTATE_X , "part/rotate_x" },
    { ENGK_PART_ROTATE_Y , "part/rotate_y" },
    { ENGK_PART_ROTATE_Z , "part/rotate_z" },
    { ENGK_PART_SCALE    , "part/scale"    },
    //The Desk
    { ENGK_DESK_TR_X     , "desk/tr_x"     },
    { ENGK_DESK_TR_Y     , "desk/tr_y"     },
    { ENGK_DESK_TR_Z     , "desk/tr_z"     },
    { ENGK_DESK_CENTER_X , "desk/center_x" },
    { ENGK_DESK_CENTER_Y , "desk/center_y" },
    { ENGK_DESK_CENTER_Z , "desk/center_z" },
    { ENGK_DESK_ROTATE_X , "desk/rotate_x" },
    { ENGK_DESK_ROTATE_Y , "desk/rotate_y" },
    { ENGK_DESK_ROTATE_Z , "desk/rotate_z" },
    { ENGK_DESK_SCALE    , "desk/scale"    },
    //The Laser Head
    { ENGK_LHEAD_TR_X      , "lhead/tr_x"       },
    { ENGK_LHEAD_TR_Y      , "lhead/tr_y"       },
    { ENGK_LHEAD_TR_Z      , "lhead/tr_z"       },
    { ENGK_LHEAD_CENTER_X  , "lhead/center_x"   },
    { ENGK_LHEAD_CENTER_Y  , "lhead/center_y"   },
    { ENGK_LHEAD_CENTER_Z  , "lhead/center_z"   },
    { ENGK_LHEAD_ROTATE_X  , "lhead/rotate_x"   },
    { ENGK_LHEAD_ROTATE_Y  , "lhead/rotate_y"   },
    { ENGK_LHEAD_ROTATE_Z  , "lhead/rotate_z"   },
    { ENGK_LHEAD_SCALE     , "lhead/scale"      },
    { ENGK_LHEAD_LSR_TR_X  , "lhead/lsr_tr_x"   },
    { ENGK_LHEAD_LSR_TR_Y  , "lhead/lsr_tr_y"   },
    { ENGK_LHEAD_LSR_TR_Z  , "lhead/lsr_tr_z"   },
    { ENGK_LHEAD_LSR_NRML_X, "lhead/lsr_nrml_x" },
    { ENGK_LHEAD_LSR_NRML_Y, "lhead/lsr_nrml_y" },
    { ENGK_LHEAD_LSR_NRML_Z, "lhead/lsr_nrml_z" },
    { ENGK_LHEAD_LSR_LENGHT, "lhead/lsr_lenght" },
    { ENGK_LHEAD_LSR_CLIP  , "lhead/lsr_clip"   },
    //The GRIP
    { ENGK_GRIP_TR_X     , "grip/tr_x"     },
    { ENGK_GRIP_TR_Y     , "grip/tr_y"     },
    { ENGK_GRIP_TR_Z     , "grip/tr_z"     },
    { ENGK_GRIP_CENTER_X , "grip/center_x" },
    { ENGK_GRIP_CENTER_Y , "grip/center_y" },
    { ENGK_GRIP_CENTER_Z , "grip/center_z" },
    { ENGK_GRIP_ROTATE_X , "grip/rotate_x" },
    { ENGK_GRIP_ROTATE_Y , "grip/rotate_y" },
    { ENGK_GRIP_ROTATE_Z , "grip/rotate_z" },
    { ENGK_GRIP_SCALE    , "grip/scale"    },
    { ENGK_GRIP_VIS      , "grip/vis"      },
    //Models
    { ENGK_MDL_PART , "model/part" },
    { ENGK_MDL_GRIP , "model/grip" },
    { ENGK_MDL_DESK , "model/desk" },
    { ENGK_MDL_LHEAD, "model/lhead" }
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

    GUI_TYPES::SGuiSettings loadGuiSettings() {
        using namespace GUI_TYPES;
        SGuiSettings res;
        settingsFile->beginGroup(GUI_PREFIX);
        //Common
        res.msaa           = readGuiValue <TMSAA>  (ENGK_MSAA);
        res.snapshotScale  = readGuiValue <TScale> (ENGK_SNAP_SCALE);
        res.snapshotWidth  = readGuiValue <size_t> (ENGK_SNAP_WIDTH);
        res.snapshotHeight = readGuiValue <size_t> (ENGK_SNAP_HEIGHT);
        settingsFile->endGroup();

        //The Part
        res.partTrX       = readGuiValue <TDistance> (ENGK_PART_TR_X);
        res.partTrY       = readGuiValue <TDistance> (ENGK_PART_TR_Y);
        res.partTrZ       = readGuiValue <TDistance> (ENGK_PART_TR_Z);
        res.partCenterX   = readGuiValue <TDistance> (ENGK_PART_CENTER_X);
        res.partCenterY   = readGuiValue <TDistance> (ENGK_PART_CENTER_Y);
        res.partCenterZ   = readGuiValue <TDistance> (ENGK_PART_CENTER_Z);
        res.partRotationX = readGuiValue <TDegree>   (ENGK_PART_ROTATE_X);
        res.partRotationY = readGuiValue <TDegree>   (ENGK_PART_ROTATE_Y);
        res.partRotationZ = readGuiValue <TDegree>   (ENGK_PART_ROTATE_Z);
        res.partScale     = readGuiValue <TScale>    (ENGK_PART_SCALE);
        //The Desk
        res.deskTrX       = readGuiValue <TDistance> (ENGK_DESK_TR_X);
        res.deskTrY       = readGuiValue <TDistance> (ENGK_DESK_TR_Y);
        res.deskTrZ       = readGuiValue <TDistance> (ENGK_DESK_TR_Z);
        res.deskCenterX   = readGuiValue <TDistance> (ENGK_DESK_CENTER_X);
        res.deskCenterY   = readGuiValue <TDistance> (ENGK_DESK_CENTER_Y);
        res.deskCenterZ   = readGuiValue <TDistance> (ENGK_DESK_CENTER_Z);
        res.deskRotationX = readGuiValue <TDegree>   (ENGK_DESK_ROTATE_X);
        res.deskRotationY = readGuiValue <TDegree>   (ENGK_DESK_ROTATE_Y);
        res.deskRotationZ = readGuiValue <TDegree>   (ENGK_DESK_ROTATE_Z);
        res.deskScale     = readGuiValue <TScale>    (ENGK_DESK_SCALE);
        //The Laser Head
        res.lheadTrX        = readGuiValue <TDistance> (ENGK_LHEAD_TR_X);
        res.lheadTrY        = readGuiValue <TDistance> (ENGK_LHEAD_TR_Y);
        res.lheadTrZ        = readGuiValue <TDistance> (ENGK_LHEAD_TR_Z);
        res.lheadCenterX    = readGuiValue <TDistance> (ENGK_LHEAD_CENTER_X);
        res.lheadCenterY    = readGuiValue <TDistance> (ENGK_LHEAD_CENTER_Y);
        res.lheadCenterZ    = readGuiValue <TDistance> (ENGK_LHEAD_CENTER_Z);
        res.lheadRotationX  = readGuiValue <TDegree>   (ENGK_LHEAD_ROTATE_X);
        res.lheadRotationY  = readGuiValue <TDegree>   (ENGK_LHEAD_ROTATE_Y);
        res.lheadRotationZ  = readGuiValue <TDegree>   (ENGK_LHEAD_ROTATE_Z);
        res.lheadScale      = readGuiValue <TScale>    (ENGK_LHEAD_SCALE);
        res.lheadLsrTrX     = readGuiValue <TDistance> (ENGK_LHEAD_LSR_TR_X);
        res.lheadLsrTrY     = readGuiValue <TDistance> (ENGK_LHEAD_LSR_TR_Y);
        res.lheadLsrTrZ     = readGuiValue <TDistance> (ENGK_LHEAD_LSR_TR_Z);
        res.lheadLsrNormalX = readGuiValue <TDistance> (ENGK_LHEAD_LSR_NRML_X);
        res.lheadLsrNormalY = readGuiValue <TDistance> (ENGK_LHEAD_LSR_NRML_Y);
        res.lheadLsrNormalZ = readGuiValue <TDistance> (ENGK_LHEAD_LSR_NRML_Z);
        res.lheadLsrLenght  = readGuiValue <TDistance> (ENGK_LHEAD_LSR_LENGHT);
        res.lheadLsrClip    = readGuiValue <bool>      (ENGK_LHEAD_LSR_CLIP);
        //The Grip
        res.gripTrX       = readGuiValue <TDistance> (ENGK_GRIP_TR_X);
        res.gripTrY       = readGuiValue <TDistance> (ENGK_GRIP_TR_Y);
        res.gripTrZ       = readGuiValue <TDistance> (ENGK_GRIP_TR_Z);
        res.gripCenterX   = readGuiValue <TDistance> (ENGK_GRIP_CENTER_X);
        res.gripCenterY   = readGuiValue <TDistance> (ENGK_GRIP_CENTER_Y);
        res.gripCenterZ   = readGuiValue <TDistance> (ENGK_GRIP_CENTER_Z);
        res.gripRotationX = readGuiValue <TDegree>   (ENGK_GRIP_ROTATE_X);
        res.gripRotationY = readGuiValue <TDegree>   (ENGK_GRIP_ROTATE_Y);
        res.gripRotationZ = readGuiValue <TDegree>   (ENGK_GRIP_ROTATE_Z);
        res.gripScale     = readGuiValue <TScale>    (ENGK_GRIP_SCALE);
        res.gripVis       = readGuiValue <bool>      (ENGK_GRIP_VIS);
        return res;
    }

    void saveGuiSettings(const GUI_TYPES::SGuiSettings &settings) {
        settingsFile->beginGroup(GUI_PREFIX);
        writeGuiValue(ENGK_MSAA         , settings.msaa);
        writeGuiValue(ENGK_SNAP_SCALE   , settings.snapshotScale);
        writeGuiValue(ENGK_SNAP_WIDTH   , settings.snapshotWidth);
        writeGuiValue(ENGK_SNAP_HEIGHT  , settings.snapshotHeight);
        settingsFile->endGroup();
        //The Part
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
        //The Desk
        writeGuiValue(ENGK_DESK_TR_X    , settings.deskTrX);
        writeGuiValue(ENGK_DESK_TR_Y    , settings.deskTrY);
        writeGuiValue(ENGK_DESK_TR_Z    , settings.deskTrZ);
        writeGuiValue(ENGK_DESK_CENTER_X, settings.deskCenterX);
        writeGuiValue(ENGK_DESK_CENTER_Y, settings.deskCenterY);
        writeGuiValue(ENGK_DESK_CENTER_Z, settings.deskCenterZ);
        writeGuiValue(ENGK_DESK_ROTATE_X, settings.deskRotationX);
        writeGuiValue(ENGK_DESK_ROTATE_Y, settings.deskRotationY);
        writeGuiValue(ENGK_DESK_ROTATE_Z, settings.deskRotationZ);
        writeGuiValue(ENGK_DESK_SCALE   , settings.deskScale);
        //The Laser Head
        writeGuiValue(ENGK_LHEAD_TR_X      , settings.lheadTrX);
        writeGuiValue(ENGK_LHEAD_TR_Y      , settings.lheadTrY);
        writeGuiValue(ENGK_LHEAD_TR_Z      , settings.lheadTrZ);
        writeGuiValue(ENGK_LHEAD_CENTER_X  , settings.lheadCenterX);
        writeGuiValue(ENGK_LHEAD_CENTER_Y  , settings.lheadCenterY);
        writeGuiValue(ENGK_LHEAD_CENTER_Z  , settings.lheadCenterZ);
        writeGuiValue(ENGK_LHEAD_ROTATE_X  , settings.lheadRotationX);
        writeGuiValue(ENGK_LHEAD_ROTATE_Y  , settings.lheadRotationY);
        writeGuiValue(ENGK_LHEAD_ROTATE_Z  , settings.lheadRotationZ);
        writeGuiValue(ENGK_LHEAD_SCALE     , settings.lheadScale);
        writeGuiValue(ENGK_LHEAD_LSR_TR_X  , settings.lheadLsrTrX);
        writeGuiValue(ENGK_LHEAD_LSR_TR_Y  , settings.lheadLsrTrY);
        writeGuiValue(ENGK_LHEAD_LSR_TR_Z  , settings.lheadLsrTrZ);
        writeGuiValue(ENGK_LHEAD_LSR_NRML_X, settings.lheadLsrNormalX);
        writeGuiValue(ENGK_LHEAD_LSR_NRML_Y, settings.lheadLsrNormalY);
        writeGuiValue(ENGK_LHEAD_LSR_NRML_Z, settings.lheadLsrNormalZ);
        writeGuiValue(ENGK_LHEAD_LSR_LENGHT, settings.lheadLsrLenght);
        writeGuiValue(ENGK_LHEAD_LSR_CLIP  , settings.lheadLsrClip);
        //The Grip
        writeGuiValue(ENGK_GRIP_TR_X    , settings.gripTrX);
        writeGuiValue(ENGK_GRIP_TR_Y    , settings.gripTrY);
        writeGuiValue(ENGK_GRIP_TR_Z    , settings.gripTrZ);
        writeGuiValue(ENGK_GRIP_CENTER_X, settings.gripCenterX);
        writeGuiValue(ENGK_GRIP_CENTER_Y, settings.gripCenterY);
        writeGuiValue(ENGK_GRIP_CENTER_Z, settings.gripCenterZ);
        writeGuiValue(ENGK_GRIP_ROTATE_X, settings.gripRotationX);
        writeGuiValue(ENGK_GRIP_ROTATE_Y, settings.gripRotationY);
        writeGuiValue(ENGK_GRIP_ROTATE_Z, settings.gripRotationZ);
        writeGuiValue(ENGK_GRIP_SCALE   , settings.gripScale);
        writeGuiValue(ENGK_GRIP_VIS     , settings.gripVis);
    }

    inline static std::string defMdlName(const GUI_TYPES::EN_ModelPurpose model) {
        switch(model) {
            using namespace GUI_TYPES;
            case ENMP_DESK   : return ":/Models/Data/Models/table_lifted.step";
            case ENMP_PART   : return ":/Models/Data/Models/45deg AdjMirr Adapter Left Rev1.STEP";
            case ENMP_LSRHEAD: return ":/Models/Data/Models/tool_camera_flir.step";
            case ENMP_GRIP   : return ":/Models/Data/Models/MHZ2_16D_grip.stp";
        }
        assert(false);
        return std::string();
    }

    inline static EN_GuiKeys keyForMdl(const GUI_TYPES::EN_ModelPurpose model) {
        switch(model) {
            using namespace GUI_TYPES;
            case ENMP_DESK   : return ENGK_MDL_DESK;
            case ENMP_PART   : return ENGK_MDL_PART;
            case ENMP_LSRHEAD: return ENGK_MDL_LHEAD;
            case ENMP_GRIP   : return ENGK_MDL_GRIP;
        }
        return ENGK_LAST;
    }

    std::string loadModelPath(const GUI_TYPES::EN_ModelPurpose model) {
        std::string result = defMdlName(model);
        auto it = guiKeyMap.find(keyForMdl(model));
        if (it != guiKeyMap.cend()) {
            settingsFile->beginGroup(GUI_PREFIX);
            if (!settingsFile->contains(it->second))
                settingsFile->setValue(it->second, QString::fromStdString(result));
            else
                result = settingsFile->value(it->second).toString().toStdString();
            settingsFile->endGroup();
        }
        return result;
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

GUI_TYPES::SGuiSettings CSimpleSettingsStorage::loadGuiSettings()
{
    return d_ptr->loadGuiSettings();
}

void CSimpleSettingsStorage::saveGuiSettings(const GUI_TYPES::SGuiSettings &settings)
{
    d_ptr->saveGuiSettings(settings);
}

std::string CSimpleSettingsStorage::loadModelPath(const GUI_TYPES::EN_ModelPurpose model)
{
    return d_ptr->loadModelPath(model);
}
