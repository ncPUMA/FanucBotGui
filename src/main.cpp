#include "mainwindow.h"

#include <QDebug>
#include <QApplication>
#include <QFile>

#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Environment.hxx>

#include "csimplesettingsstorage.h"

#include "BotSocket/cfanucbotsocket.h"

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

    QApplication a(argc, argv);

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
    settings.setSettingsFName("LBOT.INI");
#else
    settings.setSettingsFName("conf.cfg");
#endif

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
