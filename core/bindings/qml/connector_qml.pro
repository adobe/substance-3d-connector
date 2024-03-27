TEMPLATE = lib
TARGET = connector
QT -= gui
QT += qml
CONFIG += qt plugin c++14

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.allegorithmic.liveconnector

QMAKE_CXXFLAGS += -Wextra -Wpedantic

QMAKE_CC = clang
QMAKE_CXX = clang++

SOURCES = connector_qml.cpp

HEADERS = connector_qml.hpp

DISTFILES = qmldir

INCLUDEPATH = ../../include

!win32 {
    QMAKE_LFLAGS += -L. substance_connector.so -Wl,-rpath='\'\$$ORIGIN\''
}

win32 {
    LIBS += substance_connector
}
