TEMPLATE = app
CONFIG +=console c++17
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
    fortune/Beachline.cpp \
    fortune/Box.cpp \
    fortune/Event.cpp \
    fortune/FortuneAlgorithm.cpp \
    fortune/Vector2.cpp \
    fortune/VoronoiDiagram.cpp \
    test.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.

INCLUDEPATH+="D:\soft\eigen-3.4.0"
HEADERS += \
    backend.h \
    data.h \
    fortune/Arc.h \
    fortune/Beachline.h \
    fortune/Box.h \
    fortune/Event.h \
    fortune/FortuneAlgorithm.h \
    fortune/PriorityQueue.h \
    fortune/Vector2.h \
    fortune/VoronoiDiagram.h \
    mathutil.h \
    objoperator.h \
    test.h
