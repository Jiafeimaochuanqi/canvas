#ifndef DATA_H
#define DATA_H
#include <QVector>
#include <QPoint>
#include <QObject>
#include <QVariantList>
#include <iostream>
#include <assert.h>
struct InterPolyDraw {
    Q_GADGET

public:
    QVector<QPoint> pos;
    bool visible = true;
    Q_PROPERTY(bool visible MEMBER visible)
    Q_PROPERTY(QVariantList pos READ getPos)
    QVariantList getPos(){
        QVariantList l;
        l.reserve(pos.size());
        for(QPoint p:pos){
            l.append(p);
        }
        return l;
    }
};
struct InterGaussDraw {
    Q_GADGET

public:
    QVector<QPoint> pos;
    bool visible = true;
    Q_PROPERTY(bool visible MEMBER visible)
    Q_PROPERTY(QVariantList pos READ getPos)
    QVariantList getPos(){
        QVariantList l;
        l.reserve(pos.size());
        for(QPoint p:pos){
            l.append(p);
        }
        return l;
    }
    float sigma2 = 10000.0f;
};
struct LeastSquaresDraw {
    Q_GADGET

public:
    QVector<QPoint> pos;
    bool visible = true;
    Q_PROPERTY(bool visible MEMBER visible)
    Q_PROPERTY(QVariantList pos READ getPos)
    QVariantList getPos(){
        QVariantList l;
        l.reserve(pos.size());
        for(QPoint p:pos){
            l.append(p);
        }
        return l;
    }
    float lambda = 0.001f;
};
struct ParameterizationDraw {
    Q_GADGET

public:
    QVector<QPoint> pos;
    bool visible = true;
    Q_PROPERTY(bool visible MEMBER visible)
    Q_PROPERTY(QVariantList pos READ getPos)
    QVariantList getPos(){
        QVariantList l;
        l.reserve(pos.size());
        for(QPoint p:pos){
            l.append(p);
        }
        return l;
    }
};
struct ControlPoint{
    double val=0;
    double ldiff=0;
    double rdiff=0;
    bool fixed_diff = false;//首尾端点的属性影响样条计算方法
	ControlPoint(double val, double ldiff, double rdiff, bool fixed_diff):val(val), ldiff(ldiff), rdiff(rdiff), fixed_diff(fixed_diff){

	}
};
struct NodeArr {
    NodeArr():size(0) {}
    NodeArr(std::vector<double> x, std::vector<double> y) : xs(x), ys(y){size=x.size();}
    std::vector<double> xs;
    std::vector<double> ys;
    int size;
};
struct ControlPointArray2D {
    Q_GADGET

public:
    double showpara = 0.05;
    std::vector<QPointF> pos;
    //绘图的点
    std::vector<NodeArr> drawPoints;
    bool visible = true;
    Q_PROPERTY(QVariantList pos READ getPos)
    Q_PROPERTY(QVariantList control READ getControl)
    Q_PROPERTY(QVariantList leftControl READ getLeftControl)
    Q_PROPERTY(QVariantList rightControl READ getRightControl)
    QVariantList getPos(){
        QVariantList l;
        l.reserve(pos.size());
        for(QPointF p:pos){
            l.append(p);
        }
        return l;
    }
    QVariantList getControl(){
        QVariantList l;
        l.reserve(control.size());
        for(QPointF p:control){
            l.append(p);
        }
        return l;
    }
    QVariantList getLeftControl(){
        QVariantList l;
        l.reserve(leftControl.size());
        for(QPointF p:leftControl){
            l.append(p);
        }
        return l;
    }
    QVariantList getRightControl(){
        QVariantList l;
        l.reserve(rightControl.size());
        for(QPointF p:rightControl){
            l.append(p);
        }
        return l;
    }
    std::vector<ControlPoint> xs;
    std::vector<ControlPoint> ys;
    void addPoint(QPointF& p);
    void setPoint(int index,QPointF&p){
        xs[index].val=p.x();
        ys[index].val=p.y();
    }
    void setLDiff(int index,QPointF&p){
        fixed[index]=true;
        xs[index].fixed_diff=true;
        xs[index].ldiff=(xs[index].val-p.x())/showpara;
        ys[index].ldiff=(ys[index].val-p.y())/showpara;
    }
    void setRDiff(int index,QPointF&p){
        fixed[index] = true;
        xs[index].fixed_diff = true;
        ys[index].fixed_diff = true;
        xs[index].rdiff = (p.x() - xs[index].val)/showpara;
        ys[index].rdiff = (p.y() - ys[index].val)/showpara;
    }
    int nodeNum(){
        assert(xs.size()==ys.size() && ys.size()==fixed.size());
        return xs.size();
    }
    int drawSize(){
        return drawPoints.size();
    }
    QVector<QPointF> control;
    QVector<QPointF> leftControl;
    QVector<QPointF> rightControl;
    void calculateParamRange(int range_num);
    std::vector<std::vector<float>> xparam;
    std::vector<std::vector<float>> yparam;
    uint64_t nodePerRange = 20;//每段三次样条的采样数

    std::vector<bool> fixed;//影响参数计算范围
    void clear(){
        xs.clear();
        ys.clear();
        xparam.clear();
        yparam.clear();
        control.clear();
        leftControl.clear();
        rightControl.clear();
        fixed.clear();
        drawPoints.clear();
        pos.clear();
    }
};

inline void ControlPointArray2D::addPoint(QPointF &p)
{
    xs.push_back({ p.x(),0.0,0.0,false });
    ys.push_back(ControlPoint{p.y(),0.0,0.0,false});
    fixed.push_back(false);
}


/*
inline void ControlPointArray2D::calculateParamRange(int range_num)
{
    int stpoint = range_num, edpoint = range_num + 1;
    while (stpoint>=0 && fixed[stpoint] == false) {
        stpoint--;
    }
    stpoint = (stpoint<0)?0:stpoint;
    while (edpoint < fixed.size() && fixed[edpoint] == false) {
        edpoint++;
    }
    edpoint = (edpoint >= fixed.size()) ? (fixed.size()-1) : edpoint;
        std::cout << "stpoint:" << stpoint << std::endl;
        std::cout << "edpoint:" << edpoint << std::endl;
}*/


#endif // DATA_H
