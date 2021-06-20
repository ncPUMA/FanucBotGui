#include "cmainsettings.h"

#include <map>

#include <QSettings>
#include <QHostAddress>

#include "cabstractguisettings.h"
#include "BotSocket/cabstractbotsocketsettings.h"

static const char *GUI_PREFIX = "GUI";

enum EN_GuiKeys
{
    ENGK_FIRST = 0,

    ENGK_MSAA = ENGK_FIRST,

    //The LaserBot
    ENGK_BOT_ANCHOR_X,
    ENGK_BOT_ANCHOR_Y,
    ENGK_BOT_ANCHOR_Z,
    ENGK_BOT_LASER_X,
    ENGK_BOT_LASER_Y,
    ENGK_BOT_LASER_Z,
    ENGK_BOT_COORD_TYPE,

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

    //The Grip
    ENGK_GRIP_VISIBLE,
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

    ENGK_LAST
};
typedef int TGuiKey;

static const std::map <TGuiKey, QString> guiKeyMap = {
    { ENGK_MSAA          , "msaa"           },
    //The LaserBot
    { ENGK_BOT_ANCHOR_X  , "bot_anch_x"     },
    { ENGK_BOT_ANCHOR_Y  , "bot_anch_y"     },
    { ENGK_BOT_ANCHOR_Z  , "bot_anch_z"     },
    { ENGK_BOT_LASER_X   , "bot_lsr_x"      },
    { ENGK_BOT_LASER_Y   , "bot_lsr_y"      },
    { ENGK_BOT_LASER_Z   , "bot_lsr_z"      },
    { ENGK_BOT_COORD_TYPE, "bot_coord_type" },
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
    { ENGK_PART_SCALE    , "part_scale"     },
    //The Grip
    { ENGK_GRIP_VISIBLE  , "grip_visible"   },
    { ENGK_GRIP_TR_X     , "grip_tr_x"      },
    { ENGK_GRIP_TR_Y     , "grip_tr_y"      },
    { ENGK_GRIP_TR_Z     , "grip_tr_z"      },
    { ENGK_GRIP_CENTER_X , "grip_center_x"  },
    { ENGK_GRIP_CENTER_Y , "grip_center_y"  },
    { ENGK_GRIP_CENTER_Z , "grip_center_z"  },
    { ENGK_GRIP_ROTATE_X , "grip_rotate_x"  },
    { ENGK_GRIP_ROTATE_Y , "grip_rotate_y"  },
    { ENGK_GRIP_ROTATE_Z , "grip_rotate_z"  },
    { ENGK_GRIP_SCALE    , "grip_scale"     },
};

static const char *BOT_PREFIX = "BOT";
static const char *BOT_KEY_LCL_IP = "lcl_ip";
static const char *BOT_KEY_LCL_PORT = "lcl_port";
static const char *BOT_KEY_REM_IP = "rem_ip";
static const char *BOT_KEY_REM_PORT = "rem_port";

static const char *BOT_DEF_LCL_IP = "192.168.0.1";
static const uint16_t BOT_DEF_LCL_PORT = 1122;
static const char *BOT_DEF_REM_IP = "192.168.0.2";
static const uint16_t BOT_DEF_REM_PORT = 2211;

class CMainSettingsPrivate : public CAbstractGuiSettings, public CAbstractBotSocketSettings
{
    friend class CMainSettings;

public:
    CMainSettingsPrivate() :
        CAbstractGuiSettings(),
        CAbstractBotSocketSettings(),
        settings(new QSettings()) {

    }

    ~CMainSettingsPrivate() {
        delete settings;
    }

//CAbstractGuiSettings
    //CAbstractGuiSettings
    GUI_TYPES::TMSAA getMsaa() const final { return readGuiValue <GUI_TYPES::TMSAA> (ENGK_MSAA); }
    void setMsaa(const GUI_TYPES::TMSAA value) final { writeGuiValue(ENGK_MSAA, value); }

    //The LaserBot
    double getBotAnchorX() const final { return readGuiValue <double> (ENGK_BOT_ANCHOR_X); }
    double getBotAnchorY() const final { return readGuiValue <double> (ENGK_BOT_ANCHOR_Y); }
    double getBotAnchorZ() const final { return readGuiValue <double> (ENGK_BOT_ANCHOR_Z); }
    double getBotLaserX() const final { return readGuiValue <double> (ENGK_BOT_LASER_X); }
    double getBotLaserY() const final { return readGuiValue <double> (ENGK_BOT_LASER_Y); }
    double getBotLaserZ() const final { return readGuiValue <double> (ENGK_BOT_LASER_Z); }
    GUI_TYPES::TCoordSystem getBotCoordType() const  final {
        return readGuiValue <GUI_TYPES::TCoordSystem> (ENGK_BOT_COORD_TYPE);
    }

    void setBotAnchorX(const double value) final { writeGuiValue(ENGK_BOT_ANCHOR_X, value); }
    void setBotAnchorY(const double value) final { writeGuiValue(ENGK_BOT_ANCHOR_Y, value); }
    void setBotAnchorZ(const double value) final { writeGuiValue(ENGK_BOT_ANCHOR_Z, value); }
    void setBotLaserX(const double value) final { writeGuiValue(ENGK_BOT_LASER_X, value); }
    void setBotLaserY(const double value) final { writeGuiValue(ENGK_BOT_LASER_Y, value); }
    void setBotLaserZ(const double value) final { writeGuiValue(ENGK_BOT_LASER_Z, value); }
    void setBotCoordType(const GUI_TYPES::TCoordSystem value) final {
        writeGuiValue(ENGK_BOT_COORD_TYPE, value);
    }

    //The Part
    double getPartTrX() const final { return readGuiValue <double> (ENGK_PART_TR_X); }
    double getPartTrY() const final { return readGuiValue <double> (ENGK_PART_TR_Y); }
    double getPartTrZ() const final { return readGuiValue <double> (ENGK_PART_TR_Z); }
    double getPartCenterX() const final { return readGuiValue <double> (ENGK_PART_CENTER_X); }
    double getPartCenterY() const final { return readGuiValue <double> (ENGK_PART_CENTER_Y); }
    double getPartCenterZ() const final { return readGuiValue <double> (ENGK_PART_CENTER_Z); }
    double getPartRotationX() const final { return readGuiValue <double> (ENGK_PART_ROTATE_X); }
    double getPartRotationY() const final { return readGuiValue <double> (ENGK_PART_ROTATE_Y); }
    double getPartRotationZ() const final { return readGuiValue <double> (ENGK_PART_ROTATE_Z); }
    double getPartScale() const final { return readGuiValue <double> (ENGK_PART_SCALE); }

    void setPartTrX(const double value) final { writeGuiValue(ENGK_PART_TR_X, value); }
    void setPartTrY(const double value) final { writeGuiValue(ENGK_PART_TR_Y, value); }
    void setPartTrZ(const double value) final { writeGuiValue(ENGK_PART_TR_Z, value); }
    void setPartCenterX(const double value) final { writeGuiValue(ENGK_PART_CENTER_X, value); }
    void setPartCenterY(const double value) final { writeGuiValue(ENGK_PART_CENTER_Y, value); }
    void setPartCenterZ(const double value) final { writeGuiValue(ENGK_PART_CENTER_Z, value); }
    void setPartRotationX(const double value) final { writeGuiValue(ENGK_PART_ROTATE_X, value); }
    void setPartRotationY(const double value) final { writeGuiValue(ENGK_PART_ROTATE_Y, value); }
    void setPartRotationZ(const double value) final { writeGuiValue(ENGK_PART_ROTATE_Z, value); }
    void setPartScale(const double value) final { writeGuiValue(ENGK_PART_SCALE, value); }

    //The Grip
    bool   isGripVisible() const final { return readGuiValue <bool> (ENGK_GRIP_VISIBLE); }
    double getGripTrX() const final { return readGuiValue <double> (ENGK_GRIP_TR_X); }
    double getGripTrY() const final { return readGuiValue <double> (ENGK_GRIP_TR_Y); }
    double getGripTrZ() const final { return readGuiValue <double> (ENGK_GRIP_TR_Z); }
    double getGripCenterX() const final { return readGuiValue <double> (ENGK_GRIP_CENTER_X); }
    double getGripCenterY() const final { return readGuiValue <double> (ENGK_GRIP_CENTER_Y); }
    double getGripCenterZ() const final { return readGuiValue <double> (ENGK_GRIP_CENTER_Z); }
    double getGripRotationX() const final { return readGuiValue <double> (ENGK_GRIP_ROTATE_X); }
    double getGripRotationY() const final { return readGuiValue <double> (ENGK_GRIP_ROTATE_Y); }
    double getGripRotationZ() const final { return readGuiValue <double> (ENGK_GRIP_ROTATE_Z); }
    double getGripScale() const final { return readGuiValue <double> (ENGK_GRIP_SCALE); }

    void setGripVisible(const bool value) final { writeGuiValue(ENGK_GRIP_VISIBLE, value); }
    void setGripTrX(const double value) final { writeGuiValue(ENGK_GRIP_TR_X, value); }
    void setGripTrY(const double value) final { writeGuiValue(ENGK_GRIP_TR_Y, value); }
    void setGripTrZ(const double value) final { writeGuiValue(ENGK_GRIP_TR_Z, value); }
    void setGripCenterX(const double value) final { writeGuiValue(ENGK_GRIP_CENTER_X, value); }
    void setGripCenterY(const double value) final { writeGuiValue(ENGK_GRIP_CENTER_Y, value); }
    void setGripCenterZ(const double value) final { writeGuiValue(ENGK_GRIP_CENTER_Z, value); }
    void setGripRotationX(const double value) final { writeGuiValue(ENGK_GRIP_ROTATE_X, value); }
    void setGripRotationY(const double value) final { writeGuiValue(ENGK_GRIP_ROTATE_Y, value); }
    void setGripRotationZ(const double value) final { writeGuiValue(ENGK_GRIP_ROTATE_Z, value); }
    void setGripScale(const double value) final { writeGuiValue(ENGK_GRIP_SCALE, value); }

//CAbstractBotSocketSettings
    uint32_t getLocalIpV4() const { return readIp(BOT_PREFIX, BOT_KEY_LCL_IP); }
    uint16_t getLocalUdpPort() const { return readIp(BOT_PREFIX, BOT_KEY_LCL_PORT); }
    uint32_t getRemoteBotIpV4() const { return readIp(BOT_PREFIX, BOT_KEY_REM_IP); }
    uint16_t getRemoteBotUdpPort() const { return readIp(BOT_PREFIX, BOT_KEY_REM_PORT); }

private:
    template <typename T>
    T readGuiValue(const TGuiKey key) const {
        T result = T();
        auto it = guiKeyMap.find(key);
        if (it != guiKeyMap.cend()) {
            settings->beginGroup(GUI_PREFIX);
            result = settings->value(it->second).value <T> ();
            settings->endGroup();
        }
        return result;
    }

    template <typename T>
    void writeGuiValue(const TGuiKey key, const T value) {
        auto it = guiKeyMap.find(key);
        if (it != guiKeyMap.cend()) {
            settings->beginGroup(GUI_PREFIX);
            settings->setValue(it->second, QVariant::fromValue <T> (value));
            settings->endGroup();
        }
    }

    uint32_t readIp(const char *group, const char *key) const {
        QString txt;
        settings->beginGroup(group);
        txt = settings->value(key).toString();
        settings->endGroup();
        return QHostAddress(txt).toIPv4Address();
    }

    void writeIp(const char *group, const char *key, const uint32_t ip) {
        settings->beginGroup(group);
        settings->setValue(key, QHostAddress(ip).toString());
        settings->endGroup();
    }

    uint16_t readPort(const char *group, const char *key) const {
        uint16_t result = 0;
        settings->beginGroup(group);
        result = static_cast <uint16_t> (settings->value(key).toUInt());
        settings->endGroup();
        return result;
    }

    void writePort(const char *group, const char *key, const uint16_t port) {
        settings->beginGroup(group);
        settings->setValue(key, port);
        settings->endGroup();
    }

private:
    QSettings *settings;
};



CMainSettings::CMainSettings() :
    d_ptr(new CMainSettingsPrivate())
{

}

CMainSettings::~CMainSettings()
{
    delete d_ptr;
}

void CMainSettings::setSettingsFName(const char *fname)
{
    delete d_ptr->settings;
    d_ptr->settings = new QSettings(QString::fromLocal8Bit(fname), QSettings::IniFormat);

    d_ptr->settings->beginGroup(BOT_PREFIX);
    if (!d_ptr->settings->contains(BOT_KEY_LCL_IP))
        d_ptr->settings->setValue(BOT_KEY_LCL_IP, BOT_DEF_LCL_IP);
    if (!d_ptr->settings->contains(BOT_KEY_LCL_PORT))
        d_ptr->settings->setValue(BOT_KEY_LCL_PORT, BOT_DEF_LCL_PORT);
    if (!d_ptr->settings->contains(BOT_KEY_REM_IP))
        d_ptr->settings->setValue(BOT_KEY_REM_IP, BOT_DEF_REM_IP);
    if (!d_ptr->settings->contains(BOT_KEY_REM_PORT))
        d_ptr->settings->setValue(BOT_KEY_REM_PORT, BOT_DEF_REM_PORT);
    d_ptr->settings->endGroup();

    d_ptr->settings->beginGroup(GUI_PREFIX);
    if (!d_ptr->settings->contains(guiKeyMap.at(ENGK_MSAA)))
        d_ptr->settings->setValue(guiKeyMap.at(ENGK_MSAA), GUI_TYPES::ENMSAA_4);
    d_ptr->settings->endGroup();
}

CAbstractGuiSettings &CMainSettings::guiSettings()
{
    return *d_ptr;
}

CAbstractBotSocketSettings &CMainSettings::socketSettings()
{
    return *d_ptr;
}
