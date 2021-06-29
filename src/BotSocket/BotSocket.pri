SOURCES += \
    $$PWD/cabstractbotsocket.cpp \
    $$PWD/cfanucbotsocket.cpp \
    $$PWD/fanucsocket.cpp \
    $$PWD/cbotsocketimitator.cpp

HEADERS += \
    $$PWD/bot_socket_types.h \
    $$PWD/cabstractbotsocket.h \
    $$PWD/cfanucbotsocket.h \
    $$PWD/fanuc_imitation_data.h \
    $$PWD/fanucsocket.h \
    $$PWD/cabstractbotsocketsettings.h \
    $$PWD/cabstractui.h \
    $$PWD/cbotsocketimitator.h

QT += network
