QT += core testlib

CONFIG += c++14 testcase no_testcase_installs

HEADERS = catch2/catch.hpp

SOURCES += \
    test_main.cpp \
    test_point_pair_part_referencer.cpp \
    ../src/log/loguru.cpp

unix: LIBS += -ldl -lpthread

include(../src/opencascade.pri)
include(../src/PartReference/PartReference.pri)
