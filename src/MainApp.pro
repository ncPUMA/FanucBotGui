QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BotSocket/cabstractbotsocket.cpp \
    BotSocket/cabstractui.cpp \
    BotSocket/cfanucbotsocket.cpp \
    BotSocket/fanuc_relay_socket.cpp \
    BotSocket/fanuc_state_socket.cpp \
    Dialogs/CalibPoints/caddcalibpointdialog.cpp \
    Dialogs/CalibPoints/ccalibpointsorderdialog.cpp \
    Dialogs/PathPoints/caddpathpointdialog.cpp \
    Dialogs/PathPoints/cpathpointsorderdialog.cpp \
    Dialogs/TaskPoints/cbottaskdialogfacade.cpp \
    Dialogs/TaskPoints/cdrillbottaskdialog.cpp \
    Dialogs/TaskPoints/cmarkbottaskdialog.cpp \
    Dialogs/TaskPoints/cmovebottaskdialog.cpp \
    Dialogs/TaskPoints/ctaskpointsorderdialog.cpp \
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
    BotSocket/bot_socket_types.h \
    BotSocket/cabstractbotsocket.h \
    BotSocket/cabstractui.h \
    BotSocket/cfanucbotsocket.h \
    BotSocket/fanuc_relay_socket.h \
    BotSocket/fanuc_state_socket.h \
    BotSocket/simple_message.h \
    Dialogs/CalibPoints/caddcalibpointdialog.h \
    Dialogs/CalibPoints/ccalibpointsorderdialog.h \
    Dialogs/PathPoints/caddpathpointdialog.h \
    Dialogs/PathPoints/cpathpointsorderdialog.h \
    Dialogs/TaskPoints/cbottaskdialogfacade.h \
    Dialogs/TaskPoints/cdrillbottaskdialog.h \
    Dialogs/TaskPoints/cmarkbottaskdialog.h \
    Dialogs/TaskPoints/cmovebottaskdialog.h \
    Dialogs/TaskPoints/ctaskpointsorderdialog.h \
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
    Dialogs/CalibPoints/caddcalibpointdialog.ui \
    Dialogs/CalibPoints/ccalibpointsorderdialog.ui \
    Dialogs/PathPoints/caddpathpointdialog.ui \
    Dialogs/PathPoints/cpathpointsorderdialog.ui \
    Dialogs/TaskPoints/cdrillbottaskdialog.ui \
    Dialogs/TaskPoints/cmarkbottaskdialog.ui \
    Dialogs/TaskPoints/cmovebottaskdialog.ui \
    Dialogs/TaskPoints/ctaskpointsorderdialog.ui \
    cguisettingswidget.ui \
    mainwindow.ui

include(opencascade.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../.gitignore \
    Data/Icons/fps-counter.png \
    Data/Icons/open.png \
    Data/Icons/play.png \
    Data/Icons/shading.png \
    Data/Icons/stop.png \
    Data/Lamps/green.png \
    Data/Lamps/red.png \
    Data/Models/LDLSR30w.STEP \
    Data/Models/MHZ2_16D_grip.stp \
    Data/Models/Neje tool 30W Laser Module.stp \
    Data/Models/WTTGA-001 - Configurable Table.stp \
    Data/Models/gripper_v1.step \
    Data/Models/plate.stp \
    Data/Models/turbine_blade.stp \
    Data/StyleSheets/style.qss \
    ../LICENSE \
    ../README.md

RESOURCES += \
    data.qrc

include(PartReference/PartReference.pri)
