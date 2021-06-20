#ifndef CMAINSETTINGS_H
#define CMAINSETTINGS_H

class CAbstractGuiSettings;
class CAbstractBotSocketSettings;
class CMainSettingsPrivate;

class CMainSettings
{
public:
    CMainSettings();
    virtual ~CMainSettings();

    void setSettingsFName(const char *fname);

    CAbstractGuiSettings& guiSettings();
    CAbstractBotSocketSettings& socketSettings();

private:
    CMainSettings(const CMainSettings &) = delete;
    CMainSettings& operator=(const CMainSettings &) = delete;

private:
    CMainSettingsPrivate * const d_ptr;
};

#endif // CMAINSETTINGS_H
