QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Dialogs/caddcalibpointdialog.cpp \
    Dialogs/cbottaskdialogfacade.cpp \
    Dialogs/cdrillbottaskdialog.cpp \
    Dialogs/cmarkbottaskdialog.cpp \
    Dialogs/cmovebottaskdialog.cpp \
    ModelLoader/cabstractmodelloader.cpp \
    ModelLoader/cbreploader.cpp \
    ModelLoader/cigesloader.cpp \
    ModelLoader/cmodelloaderfactorymethod.cpp \
    ModelLoader/cobjloader.cpp \
    ModelLoader/csteploader.cpp \
    ModelLoader/cstlloader.cpp \
    Primitives/cbotcross.cpp \
    Primitives/claservec.cpp \
    caspectwindow.cpp \
    cguisettingswidget.cpp \
    cinteractivecontext.cpp \
    cmainviewport.cpp \
    csimplesettingsstorage.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Dialogs/caddcalibpointdialog.h \
    Dialogs/cbottaskdialogfacade.h \
    Dialogs/cdrillbottaskdialog.h \
    Dialogs/cmarkbottaskdialog.h \
    Dialogs/cmovebottaskdialog.h \
    ModelLoader/cabstractmodelloader.h \
    ModelLoader/cbreploader.h \
    ModelLoader/cigesloader.h \
    ModelLoader/cmodelloaderfactorymethod.h \
    ModelLoader/cobjloader.h \
    ModelLoader/csteploader.h \
    ModelLoader/cstlloader.h \
    Primitives/cbotcross.h \
    Primitives/claservec.h \
    cabstractsettingsstorage.h \
    caspectwindow.h \
    cguisettingswidget.h \
    cinteractivecontext.h \
    cmainviewport.h \
    csimplesettingsstorage.h \
    gui_types.h \
    mainwindow.h \
    sguisettings.h

FORMS += \
    Dialogs/caddcalibpointdialog.ui \
    Dialogs/cdrillbottaskdialog.ui \
    Dialogs/cmarkbottaskdialog.ui \
    Dialogs/cmovebottaskdialog.ui \
    cguisettingswidget.ui \
    mainwindow.ui

include(opencascade.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore \
    Data/Icons/fps-counter.png \
    Data/Icons/open.png \
    Data/Icons/play.png \
    Data/Icons/shading.png \
    Data/Icons/stop.png \
    Data/Lamps/green.png \
    Data/Lamps/red.png \
    Data/Models/LDLSR30w.STEP \
    Data/Models/MHZ2_16D_grip.stp \
    Data/Models/WTTGA-001 - Configurable Table.stp \
    Data/Models/gripper_v1.step \
    Data/Models/plate.stp \
    Data/Models/turbine_blade.stp \
    Data/StyleSheets/style.qss \
    LICENSE \
    README.md

RESOURCES += \
    data.qrc

include(if/if.pri)
include(PartReference/PartReference.pri)
include(RobotMovers/RobotMovers.pri)
include(BotSocket/BotSocket.pri)
