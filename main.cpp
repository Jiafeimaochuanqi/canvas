#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <backend.h>
class Test{
public:
    Test(int* xx):xx(xx){

    }
    Test( double y,float* cc){

    }
private:
    int *xx;
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    float* ww;
    std::unique_ptr<Test> tt=std::make_unique<Test>(3.0,ww);
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
