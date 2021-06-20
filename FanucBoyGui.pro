QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BotSocket/cabstractbotsocket.cpp \
    BotSocket/cfanucbotsocket.cpp \
    BotSocket/fanucsocket.cpp \
    BotSocket/cbotsocketimitator.cpp \
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
    cmainsettings.cpp \
    cmainviewport.cpp \
    cmodelmover.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    BotSocket/bot_socket_types.h \
    BotSocket/cabstractbotsocket.h \
    BotSocket/cfanucbotsocket.h \
    BotSocket/fanuc_imitation_data.h \
    BotSocket/fanucsocket.h \
    BotSocket/cabstractbotsocketsettings.h \
    BotSocket/cabstractui.h \
    BotSocket/cbotsocketimitator.h \
    ModelLoader/cabstractmodelloader.h \
    ModelLoader/cbreploader.h \
    ModelLoader/cigesloader.h \
    ModelLoader/cmodelloaderfactorymethod.h \
    ModelLoader/cobjloader.h \
    ModelLoader/csteploader.h \
    ModelLoader/cstlloader.h \
    Primitives/cbotcross.h \
    Primitives/claservec.h \
    cabstractguisettings.h \
    caspectwindow.h \
    cguisettingswidget.h \
    cmainsettings.h \
    cmainviewport.h \
    cmodelmover.h \
    gui_types.h \
    mainwindow.h

FORMS += \
    cguisettingswidget.ui \
    mainwindow.ui

INCLUDEPATH += $$quote($$(CSF_OCCTIncludePath))
LIBS += -L$$quote($$(CSF_OCCTLibPath))

LIBS += -lTKernel -lTKMath -lTKService -lTKV3d -lTKOpenGl \
        -lTKBRep -lTKIGES -lTKSTL -lTKVRML -lTKSTEP -lTKSTEPAttr -lTKSTEP209 \
        -lTKSTEPBase -lTKGeomBase -lTKGeomAlgo -lTKG3d -lTKG2d \
        -lTKXSBase -lTKShHealing -lTKHLR -lTKTopAlgo -lTKMesh -lTKPrim \
        -lTKCDF -lTKBool -lTKBO -lTKFillet -lTKOffset -lTKLCAF -lTKCAF -lTKVCAF \
                -lTKBin -lTKXml -lTKRWMesh

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
    Data/Models/gripper_v1.step \
    Data/Models/plate.stp \
    Data/Models/turbine_blade.stp \
    Data/StyleSheets/style.qss \
    LICENSE \
    README.md

RESOURCES += \
    data.qrc
