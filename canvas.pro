TEMPLATE = app

QT += qml quick core

SOURCES += main.cpp \
    backend.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
INCLUDEPATH+="D:\soft\eigen-3.4.0"
HEADERS += \
    backend.h \
    data.h \
    mathutil.h
