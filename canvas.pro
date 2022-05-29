TEMPLATE = app
config+=console
QT +=  qml quick core
SOURCES += main.cpp \
    backend.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.

INCLUDEPATH+="D:\soft\eigen-3.4.0"
HEADERS += \
    adk/fortune/struct.h \
    adk/fortune/voronoi2d.h \
    backend.h \
    data.h \
    mathutil.h
