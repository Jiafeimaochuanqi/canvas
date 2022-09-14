#ifndef BACKEND_H
#define BACKEND_H
#include <QObject>
#include <QList>
#include <QPoint>
#include <memory>
#include <queue>
#include <fortune/FortuneAlgorithm.h>
#include "data.h"
#include "geometry/Voronoi/VoronoiMesh2.h"
//派生自QObject
//使用qmlRegisterType注册到QML中
using namespace HullDelaunayVoronoi::Primitives;
class Backend : public QObject
{
    Q_OBJECT
        //注册属性，使之可以在QML中访问--具体语法百度Q_PROPERTY
        Q_PROPERTY(QVariantList input READ getInput NOTIFY inputChanged)
        Q_PROPERTY(QVariantList output READ getOutput NOTIFY inputChanged)
        Q_PROPERTY(QVariantList control READ getControl NOTIFY inputChanged)
        Q_PROPERTY(InterPolyDraw interPoly READ getInterPoly WRITE setInterPoly NOTIFY interPolyChanged)
        Q_PROPERTY(InterGaussDraw interGauss READ getInterGauss WRITE setInterGauss NOTIFY interGaussChanged)
        Q_PROPERTY(LeastSquaresDraw leastSquares READ getLeastSquares WRITE setLeastSquares NOTIFY leastSquaresChanged)
        Q_PROPERTY(LeastSquaresDraw regression READ getRegression WRITE setRegression NOTIFY leastSquaresChanged)
        Q_PROPERTY(FittingType fittingType READ getFittingType WRITE setFittingType)
        Q_PROPERTY(ParameterizationType parameterizationType READ getParameterizationType WRITE setParameterizationType)
        Q_PROPERTY(CurveType curveType READ getCurveType WRITE setCurveType)
        Q_PROPERTY(ParameterizationDraw uniform READ getUniform WRITE setUniform NOTIFY uniformChanged)
        Q_PROPERTY(ParameterizationDraw chordal READ getChordal WRITE setChordal NOTIFY chordalChanged)
        Q_PROPERTY(ParameterizationDraw centripetal READ getCentripetal WRITE setCentripetal NOTIFY centripetalChanged)
        Q_PROPERTY(ParameterizationDraw foley READ getFoley WRITE setFoley NOTIFY foleyChanged)
        Q_PROPERTY(ControlPointArray2D controlArray READ getControlArray WRITE setControlArray NOTIFY controlArrayChanged)
        Q_PROPERTY(bool change READ getChange WRITE setChange)
        Q_PROPERTY(int moveNodeNum READ getMoveNodeNum WRITE setMoveNodeNum)
        Q_PROPERTY(int width READ getWidth WRITE setWidth)
        Q_PROPERTY(int height READ getHeight WRITE setHeight)
        Q_PROPERTY(QVariant centroid READ getCentroid)
public:
    enum FittingType{CubicSpline=0,LeastSquares,Regression,Gauss};
    Q_ENUMS(FittingType)
    enum ParameterizationType{Uniform=0,Chordal,Centripetal,Foley};
    enum CurveType{Bspline=0,Bezier};
    Q_ENUMS(ParameterizationType)
    Q_ENUMS(CurveType)
    Backend(QObject *parent=0);
    ~Backend();
    QVariantList getInput() const;
    QVariantList getOutput() const;
    QVariantList getControl() const;
    const InterPolyDraw& getInterPoly() const;
    const LeastSquaresDraw& getLeastSquares() const;
    const LeastSquaresDraw& getRegression() const;
    const FittingType& getFittingType()const;
    void setFittingType(const FittingType &fittingType);
    void setInterPoly(const InterPolyDraw& val);
    void setLeastSquares(const LeastSquaresDraw& val);
    void setRegression(const LeastSquaresDraw& val);

    void setInput(const QVector<QPointF> &newInput);
    Q_INVOKABLE void clearInput();//功能为处理数据
    Q_INVOKABLE void addInput(QPointF p);//功能为处理数据
    Q_INVOKABLE void addPoint(QPointF p);//功能为处理数据
    Q_INVOKABLE void fitting(float start,float end,float step);//功能为处理数据
    Q_INVOKABLE int findSuitableCtrlPoint(QPointF p);//功能为处理数据
    Q_INVOKABLE void setControl(QPointF p);//功能为处理数据
    Q_INVOKABLE void parameterization();//功能为处理数据
    Q_INVOKABLE void parameterizationDynamic();//功能为处理数据
    Q_INVOKABLE void bezier();//功能为处理数据
    Q_INVOKABLE void process();
    Q_INVOKABLE void quickHull2d();
    Q_INVOKABLE void playHull2d();
    Q_INVOKABLE void delaunay2d();
    Q_INVOKABLE void playDelaunay2d();
    Q_INVOKABLE void voronoi2d();
    const InterGaussDraw &getInterGauss() const;
    void setInterGauss(const InterGaussDraw &newInterGauss);

    const ParameterizationDraw &getUniform() const;
    void setUniform(const ParameterizationDraw &newUniform);

    const ParameterizationDraw &getChordal() const;
    void setChordal(const ParameterizationDraw &newChordal);

    const ParameterizationDraw &getCentripetal() const;
    void setCentripetal(const ParameterizationDraw &newCentripetal);

    const ParameterizationDraw &getFoley() const;
    void setFoley(const ParameterizationDraw &newFoley);
    QVector<QPoint> CubicSpline2Order(const std::vector<float> &x, const std::vector<float> &f,const std::vector<float> &ts,float step);

    const ParameterizationType &getParameterizationType() const;
    void setParameterizationType(const ParameterizationType &newParameterizationType);

    bool getChange() const;
    void setChange(bool newChange);

    ControlPointArray2D getControlArray() const;
    void setControlArray(ControlPointArray2D newControlArray);

    int getMoveNodeNum() const;
    void setMoveNodeNum(int newMoveNodeNum);

    const CurveType &getCurveType() const;
    void setCurveType(const CurveType &newCurveType);

    int getWidth() const;
    void setWidth(int newWidth);
    int getHeight() const;
    void setHeight(int newHeight);

    QVariant getCentroid() const;

signals:
    //信号可以在QML中访问
    void inputChanged(QVector<QPointF> input);
    void interPolyChanged(InterPolyDraw interPoly);
    void interGaussChanged(InterGaussDraw interGauss);
    void leastSquaresChanged(LeastSquaresDraw leastSquares);
    void regressionChanged(LeastSquaresDraw leastSquares);

    void uniformChanged(ParameterizationDraw uniform);
    void chordalChanged(ParameterizationDraw chordal);
    void centripetalChanged(ParameterizationDraw centripetal);
    void foleyChanged(ParameterizationDraw foley);
    void controlArrayChanged(ControlPointArray2D controlArray);

private:
    float toTk(float y);
    bool change;
    FittingType fittingType;
    ParameterizationType parameterizationType;
    CurveType curveType;
    QVector<QPointF> input;
    QVector<QPointF> output;
    QPointF centroid;
    std::vector<float> inputX;
    std::vector<float> inputY;
    InterPolyDraw interPoly;
    InterGaussDraw interGauss;
    LeastSquaresDraw leastSquares;
    LeastSquaresDraw regression;

    ParameterizationDraw uniform;
    ParameterizationDraw chordal;
    ParameterizationDraw centripetal;
    ParameterizationDraw foley;
    ControlPointArray2D controlArray;
    int moveNodeNum=-1;
    double maxChoosedist = 10;

    void calculateParamRange(int range_num);
    void calculateRange(int index);
    void updateDrawArray();
    void updateCtrlPoints();
    std::vector<float> ts;
    NodeArr drawArr;

    QVector<QPointF> points;
    int width;
    int height;
private:
     std::queue<std::vector<HullDelaunayVoronoi::Hull::Simplex<Vertex2>*>> animation;
     std::queue<std::vector<double>> centers;
     bool InBound(const Vertex2& v);
};

#endif // BACKEND_H
