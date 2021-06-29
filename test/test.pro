QT += core testlib

CONFIG += c++14 testcase no_testcase_installs

HEADERS = catch2/catch.hpp

SOURCES += \
    test_main.cpp \
    test_point_pair_part_referencer.cpp

include(../src/opencascade.pri)
include(../src/if/if.pri)
include(../src/PartReference/PartReference.pri)
include(../src/RobotMovers/RobotMovers.pri)
include(../src/BotSocket/BotSocket.pri)
