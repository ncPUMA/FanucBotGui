#include "mainwindow.h"

#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QSettings>

#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Environment.hxx>

#include "csimplesettingsstorage.h"

#include "BotSocket/cfanucbotsocket.h"
#include "log/loguru.hpp"

int main(int argc, char *argv[])
{
    //arg parsing
    bool bStyleSheet = true;
    for(int i = 0; i < argc; ++i)
    {
        const char *arg = argv[i];
        if (strcmp(arg, "--no-ssheet") == 0)
            bStyleSheet = false;
    }

    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::init(argc, argv);
    loguru::add_file("log.txt", loguru::Truncate, loguru::Verbosity_MAX);

    QApplication a(argc, argv);
    a.setFont(QFont(a.font().family(), 16));

    static Handle(OpenGl_GraphicDriver) aGraphicDriver;
    if (aGraphicDriver.IsNull())
    {
        Handle(Aspect_DisplayConnection) aDisplayConnection;
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
        aDisplayConnection = new Aspect_DisplayConnection(OSD_Environment("DISPLAY").Value());
#endif
        aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);
    }

    CSimpleSettingsStorage settings;
#ifdef Q_OS_WIN
    const char *settings_fname = "LBOT.INI";
#else
    const char *settings_fname = "conf.cfg";
#endif
    settings.setSettingsFName(settings_fname);

    MainWindow w;
    CFanucBotSocket bot_socket;
    w.init(*aGraphicDriver);
    w.setSettingsStorage(settings);
    w.setBotSocket(bot_socket);
    if (bStyleSheet)
    {
        QFile f(":/Styles/Data/StyleSheets/style.qss");
        if (f.open(QIODevice::ReadOnly))
        {
            const QString str = QString(f.readAll());
            w.setStyleSheet(str);
            f.close();
        }
    }
    w.show();
    const int retCode = a.exec();
    return retCode;
}
