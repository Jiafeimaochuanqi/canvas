#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <backend.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //qmlRegisterType注册C++类型至QML
    //arg1:import时模块名
    //arg2:主版本号次版本号
    //arg3:
    //arg4:QML类型名
    qmlRegisterType<Backend>("MyCppObject",1,0,"CppObject");
    //qRegisterMetaType<Backend::FittingType>("FittingType");
    qRegisterMetaType<InterPolyDraw>("InterPolyDraw");
    qRegisterMetaType<InterGaussDraw>("InterGaussDraw");
    qRegisterMetaType<LeastSquaresDraw>("LeastSquaresDraw");
    qRegisterMetaType<ParameterizationDraw>("ParameterizationDraw");
    qRegisterMetaType<ControlPointArray2D>("ControlPointArray2D");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));


    //auto p = root->findChild<QObject*>("mainPageLoader");
    //engine.rootContext()->setContextProperty("MainLoader",p);
    //p->setProperty("source","qrc:/HW1.qml");
    return app.exec();
}
