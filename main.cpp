#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <backend.h>
#include "test.h"

int main(int argc, char *argv[])
{
    Test::testUnique();
    std::cout << __cplusplus << std::endl;
    std::cout << __cpp_static_assert << std::endl;
    //Asserts floating point compatibility at compile time
    #if __cplusplus >= 201703L
        static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required");
    #endif
    //C99
    float negative_infinity1 = -INFINITY;
    float negative_infinity2 = -1 * INFINITY;

    float negative_infinity3 = -std::numeric_limits<float>::infinity();
    float negative_infinity4 = -1 * std::numeric_limits<float>::infinity();

    assert(std::isinf(negative_infinity1) && negative_infinity1 < std::numeric_limits<float>::lowest());
    assert(std::isinf(negative_infinity2) && negative_infinity2 < std::numeric_limits<float>::lowest());
    assert(std::isinf(negative_infinity3) && negative_infinity3 < std::numeric_limits<float>::lowest());
    assert(std::isinf(negative_infinity4) && negative_infinity4 < std::numeric_limits<float>::lowest());
    //    Test::testConvexHull3D();
    //    Test::testConvexHull3DInputObj("data\\cow.obj");
    //    Test::testConvexHull3DInputObj("data\\armadillo.obj");
    //    Test::testConvexHull3DInputObj("data\\happy.obj");
    //    Test::testConvexHull3DInputObj("data\\stanford-bunny.obj");
    //    Test::testConvexHull3DInputObj("data\\teapot.obj");
    //Test::testSphereConvexHull3D();
    //Test::testVoronoi3DInputObj("data\\armadillo.obj");
    QGuiApplication app(argc, argv);
    //qmlRegisterType注册C++类型至QML
    //arg1:import时模块名
    //arg2:主版本号次版本号
    //arg3:
    //arg4:QML类型名
    qmlRegisterType<Backend>("MyCppObject",1,0,"CppObject");

    qRegisterMetaType<InterPolyDraw>("InterPolyDraw");
    qRegisterMetaType<InterGaussDraw>("InterGaussDraw");
    qRegisterMetaType<LeastSquaresDraw>("LeastSquaresDraw");
    qRegisterMetaType<ParameterizationDraw>("ParameterizationDraw");
    qRegisterMetaType<ControlPointArray2D>("ControlPointArray2D");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));


//    auto p = root->findChild<QObject*>("mainPageLoader");
//    engine.rootContext()->setContextProperty("MainLoader",p);
//    p->setProperty("source","qrc:/HW1.qml");
    return app.exec();
}
