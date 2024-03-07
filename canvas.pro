TEMPLATE = app
CONFIG +=console c++11
QT +=  qml quick core
OBJECTS_DIR=$${PWD}/build
INCLUDEPATH+=D:\soft\tinyobjloader\install\include
CONFIG(debug, debug|release) {
    LIBS+=-LD:\soft\tinyobjloader\install\lib -ltinyobjloaderd

}else{
    LIBS+=-LD:\soft\tinyobjloader\install\lib -ltinyobjloader
}
include(D:/soft/MyCGAL/geometry.pri)

SOURCES += main.cpp \
    backend.cpp \
    test.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.

INCLUDEPATH+="D:\soft\eigen-3.4.0"
HEADERS += \
    backend.h \
    data.h \
    mathutil.h \
    objoperator.h \
    test.h
