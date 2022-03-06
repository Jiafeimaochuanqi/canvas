#include "backend.h"
#include <QVariantList>
#include <QDebug>
#include "mathutil.h"
Backend::Backend(QObject *parent): QObject(parent)
{
    fittingType=FittingType::Gauss;
    parameterizationType=ParameterizationType::Foley;
    change=false;
}

QVariantList Backend::getInput() const
{
    QVariantList l;
    l.reserve(input.size());
    for(QPointF p:input){
        l.append(p);
    }
    return l;
}

const InterPolyDraw& Backend::getInterPoly() const
{
    return interPoly;
}

const LeastSquaresDraw &Backend::getLeastSquares() const
{
    return leastSquares;
}

const LeastSquaresDraw &Backend::getRegression() const
{
    return regression;
}

const Backend::FittingType &Backend::getFittingType() const
{
    return fittingType;
}

void Backend::setFittingType(const FittingType &fittingType)
{
    this->fittingType=fittingType;
}

void Backend::setInterPoly(const InterPolyDraw &val)
{
    interPoly=val;
}

void Backend::setLeastSquares(const LeastSquaresDraw &val)
{
    leastSquares=val;
}

void Backend::setRegression(const LeastSquaresDraw &val)
{
    regression=val;
}


void Backend::setInput(const QVector<QPointF> &newInput)
{
    input = newInput;
    emit inputChanged(newInput);
}

void Backend::clearInput()
{
    input.clear();
    inputX.clear();
    inputY.clear();
    interPoly.pos.clear();
    interGauss.pos.clear();
    leastSquares.pos.clear();
    regression.pos.clear();

    uniform.pos.clear();
    chordal.pos.clear();
    centripetal.pos.clear();
    foley.pos.clear();
    controlArray.clear();
    emit inputChanged(input);
}

void Backend::addInput(QPointF p)
{

    input.push_back(p);
    inputX.push_back(p.x());
    inputY.push_back(p.y());
    controlArray.addPoint(p);
    if (controlArray.nodeNum() >= 2) {
        controlArray.drawPoints.push_back(NodeArr());
        controlArray.xparam.push_back(std::vector<float>(4));
        controlArray.yparam.push_back(std::vector<float>(4));

    }
    emit inputChanged(input);
}

void Backend::fitting(float start,float end,float step)
{
    if(input.size()<=2)return;
    std::vector<Eigen::Vector2f> inpos;
    inpos.resize(input.size());
    for(int i=0;i<input.size();i++){
        inpos[i].x()=input[i].x();
        inpos[i].y()=input[i].y();
    }
    std::vector<Eigen::Vector2f> result1=MathUtil<float>::InterpolationPolygon(inpos,start,end,step);
    interPoly.pos.resize(result1.size());
    for(int i=0;i<result1.size();i++){
        interPoly.pos[i].setX(result1[i].x());
        interPoly.pos[i].setY(result1[i].y());
    }
    emit(interPolyChanged(interPoly));
    std::vector<Eigen::Vector2f> result2=MathUtil<float>::ApproximationPolygon(inpos,inpos.size()-1,start,end,step);
    leastSquares.pos.resize(result2.size());
    for(int i=0;i<result2.size();i++){
        leastSquares.pos[i].setX(result2[i].x());
        leastSquares.pos[i].setY(result2[i].y());
    }
    emit(leastSquaresChanged(leastSquares));

    std::vector<Eigen::Vector2f> result3=MathUtil<float>::ApproximationPolygon(inpos,inpos.size()-1,start,end,step,regression.lambda);
    regression.pos.resize(result3.size());
    for(int i=0;i<result3.size();i++){
        regression.pos[i].setX(result3[i].x());
        regression.pos[i].setY(result3[i].y());
    }
    emit(regressionChanged(regression));

    std::vector<Eigen::Vector2f> result4=MathUtil<float>::InterpolationGauss(inpos,interGauss.sigma2,inpos.size()-1,start,end,step);
    interGauss.pos.resize(result4.size());
    for(int i=0;i<result3.size();i++){
        interGauss.pos[i].setX(result4[i].x());
        interGauss.pos[i].setY(result4[i].y());
    }
    emit(interGaussChanged(interGauss));

}

int Backend::findSuitableCtrlPoint(QPointF p)
{
    int res = -1;
    double minv = 1e10;
    for (int i = 0; i < controlArray.control.size(); ++i) {
        double dist = sqrt(pow(p.x() - controlArray.control[i].x(), 2) + pow(p.y() - controlArray.control[i].y(), 2));
        if (dist < minv && dist < maxChoosedist) {
            minv = dist;
            res = i;
        }
    }
    for (int i = 1; i < controlArray.leftControl.size(); ++i) {
        double dist = sqrt(pow(p.x() - controlArray.leftControl[i].x(), 2) + pow(p.y() - controlArray.leftControl[i].y(), 2));
        if (dist < minv && dist < maxChoosedist) {
            minv = dist;
            res = i+controlArray.nodeNum();
        }
    }
    for (int i = 0; i < controlArray.rightControl.size()-1; ++i) {
        double dist = sqrt(pow(p.x() - controlArray.rightControl[i].x(), 2) + pow(p.y() - controlArray.rightControl[i].y(), 2));
        if (dist < minv && dist < maxChoosedist) {
            minv = dist;
            res = i + 2 *controlArray.nodeNum();
        }
    }
    moveNodeNum=res;
    return res;
}

void Backend::setControl(QPointF p)
{
    if(moveNodeNum==-1)return;
    if (moveNodeNum < controlArray.nodeNum()){
        input[moveNodeNum]=p;
        inputX[moveNodeNum]=p.x();
        inputY[moveNodeNum]=p.y();
        controlArray.setPoint(moveNodeNum,p);
        if (moveNodeNum > 0) {
            calculateParamRange(moveNodeNum - 1);
        }
        else if (moveNodeNum == 0) {
            calculateParamRange(0);
        }
        if (moveNodeNum < controlArray.nodeNum() - 1) {
            calculateParamRange(moveNodeNum);
        }
    }
    else if (moveNodeNum < 2 * controlArray.nodeNum()){
        int index=moveNodeNum - controlArray.nodeNum();
        controlArray.setLDiff(index,p);
        if (index > 0) {
            calculateParamRange(index - 1);
        }
    }
    else{
        controlArray.setRDiff(moveNodeNum - 2 * controlArray.nodeNum(),p);
        int index=moveNodeNum - 2*controlArray.nodeNum();
        if (index < controlArray.fixed.size() - 1) {
            calculateParamRange(index);
        }
    }
}

void Backend::parameterization()
{
    float step=0.001;
    if(input.size()<=2)return;
    std::vector<float> in_pos_t= MathUtil<float>::ParameterizationUniform(inputX, inputY);
    uniform.pos=CubicSpline2Order(inputX,inputY,in_pos_t,step);
    emit(uniformChanged(uniform));
    in_pos_t = MathUtil<float>::ParameterizationChordal(inputX, inputY);
    chordal.pos=CubicSpline2Order(inputX,inputY,in_pos_t,step);
    emit(chordalChanged(chordal));
    in_pos_t = MathUtil<float>::ParameterizationCentripetal(inputX, inputY);
    centripetal.pos=CubicSpline2Order(inputX,inputY,in_pos_t,step);
    emit(centripetalChanged(centripetal));
    in_pos_t = MathUtil<float>::ParameterizationFoley(inputX, inputY);
    foley.pos=CubicSpline2Order(inputX,inputY,in_pos_t,step);
    emit(foleyChanged(foley));
}

void Backend::parameterizationDynamic()
{
    if (input.size() < 2)return;
    switch (parameterizationType) {
    case Uniform:
        ts = MathUtil<float>::ParameterizationUniform(inputX, inputY);
        break;
    case Chordal:
        ts = MathUtil<float>::ParameterizationChordal(inputX, inputY);
        break;
    case Centripetal:
        ts = MathUtil<float>::ParameterizationCentripetal(inputX, inputY);
        break;
    case Foley:
        ts = MathUtil<float>::ParameterizationFoley(inputX, inputY);
        break;
    }
    updateCtrlPoints();
    if (controlArray.nodeNum() >= 2) {
        calculateParamRange(controlArray.xparam.size() - 1);
    }
}

const InterGaussDraw &Backend::getInterGauss() const
{
    return interGauss;
}

void Backend::setInterGauss(const InterGaussDraw &newInterGauss)
{
    interGauss = newInterGauss;
}

const ParameterizationDraw &Backend::getUniform() const
{
    return uniform;
}

void Backend::setUniform(const ParameterizationDraw &newUniform)
{
    uniform = newUniform;
}

const ParameterizationDraw &Backend::getChordal() const
{
    return chordal;
}

void Backend::setChordal(const ParameterizationDraw &newChordal)
{
    chordal = newChordal;
}

const ParameterizationDraw &Backend::getCentripetal() const
{
    return centripetal;
}

void Backend::setCentripetal(const ParameterizationDraw &newCentripetal)
{
    centripetal = newCentripetal;
}

const ParameterizationDraw &Backend::getFoley() const
{
    return foley;
}

void Backend::setFoley(const ParameterizationDraw &newFoley)
{
    foley = newFoley;
}

QVector<QPoint> Backend::CubicSpline2Order(const std::vector<float> &x, const std::vector<float> &f,const std::vector<float> &ts,float step)
{
    std::vector<float> xparam=MathUtil<float>::ThreeBlending2Order(ts,inputX,0,inputX.size());
    std::vector<float> yparam=MathUtil<float>::ThreeBlending2Order(ts,inputY,0,inputY.size());
    QVector<QPoint> result;
    int k=0;
    float dt,cx,cy;
    for(;k<ts.size()-1;k++){
        for (float sampleT = ts[k]; sampleT < ts[k+1]; sampleT += step){
            float dt = sampleT - ts[k];
            float cx = xparam[4*k] + xparam[4*k+1] * dt + xparam[4*k+2] * dt * dt + xparam[4*k+3] * dt * dt * dt;
            float cy = yparam[4*k] + yparam[4*k+1] * dt + yparam[4*k+2] * dt * dt + yparam[4*k+3] * dt * dt * dt;
            result.push_back(QPoint(cx,cy));
        }
        if(k==ts.size()-2){
            dt = 1.0 - ts[ts.size()-2];
            cx= xparam[4*k] + xparam[4*k+1] * dt + xparam[4*k+2] * dt * dt + xparam[4*k+3] * dt * dt * dt;
            cy = yparam[4*k] + yparam[4*k+1] * dt + yparam[4*k+2] * dt * dt + yparam[4*k+3] * dt * dt * dt;
            result.push_back(QPoint(cx,cy));
        }
    }


    return result;
}



const Backend::ParameterizationType &Backend::getParameterizationType() const
{
    return parameterizationType;
}

void Backend::setParameterizationType(const ParameterizationType &newParameterizationType)
{
    parameterizationType = newParameterizationType;
}

bool Backend::getChange() const
{
    return change;
}

void Backend::setChange(bool newChange)
{
    change = newChange;
}

ControlPointArray2D Backend::getControlArray() const
{
    return controlArray;
}

void Backend::setControlArray(ControlPointArray2D newControlArray)
{
    controlArray = newControlArray;
}

int Backend::getMoveNodeNum() const
{
    return moveNodeNum;
}

void Backend::setMoveNodeNum(int newMoveNodeNum)
{
    moveNodeNum = newMoveNodeNum;
}
void Backend::calculateParamRange(int range_num)
{
    int stpoint = range_num, edpoint = range_num + 1;
    while (stpoint>=0 && controlArray.fixed[stpoint] == false) {
        stpoint--;
    }
    stpoint = (stpoint<0)?0:stpoint;
    while (edpoint < controlArray.fixed.size() && controlArray.fixed[edpoint] == false) {
        edpoint++;
    }
    edpoint = (edpoint >= controlArray.fixed.size()) ? (controlArray.fixed.size()-1) : edpoint;
    std::vector<float> rangeXParam;
    std::vector<float> rangeYParam;
    if (controlArray.xs[stpoint].fixed_diff == true||controlArray.xs[edpoint].fixed_diff == true) {
        rangeXParam=MathUtil<float>::ThreeBlendingOneOrder(ts,inputX,stpoint,edpoint+1,controlArray.xs[stpoint].rdiff,controlArray.xs[edpoint].ldiff);
        rangeYParam=MathUtil<float>::ThreeBlendingOneOrder(ts,inputY,stpoint,edpoint+1,controlArray.ys[stpoint].rdiff,controlArray.ys[edpoint].ldiff);
    }else{
        rangeXParam=MathUtil<float>::ThreeBlending2Order(ts,inputX,stpoint,edpoint+1);
        rangeYParam=MathUtil<float>::ThreeBlending2Order(ts,inputY,stpoint,edpoint+1);
    }
    //std::cout << "stpoint:" << stpoint << std::endl;
    //std::cout << "edpoint:" << edpoint << std::endl;
    for (int i = stpoint; i <= edpoint - 1; ++i) {
        int b = (i - stpoint) * 4;
        controlArray.xparam[i] ={rangeXParam[b], rangeXParam[b + 1], rangeXParam[b + 2], rangeXParam[b + 3]};
        controlArray.yparam[i] ={rangeYParam[b], rangeYParam[b + 1], rangeYParam[b + 2], rangeYParam[b + 3]};
        calculateRange(i);
    }
    updateDrawArray();
    updateCtrlPoints();
}

void Backend::calculateRange(int i)
{
    double t=ts[i+1]-ts[i];
    std::vector<double> _xs(controlArray.nodePerRange), _ys(controlArray.nodePerRange);
    for (int k = 0; k < controlArray.nodePerRange; ++k) {
        double curT = (k * t / controlArray.nodePerRange);
        _xs[k] = controlArray.xparam[i][3] * pow(curT, 3) + controlArray.xparam[i][2] * pow(curT, 2) + controlArray.xparam[i][1] * curT + controlArray.xparam[i][0];
        _ys[k] = controlArray.yparam[i][3] * pow(curT, 3) + controlArray.yparam[i][2] * pow(curT, 2) + controlArray.yparam[i][1] * curT + controlArray.yparam[i][0];
    }
    if(i==controlArray.nodeNum()-2){
        double curT = ts[i+1]-ts[i];
        double cx=controlArray.xparam[i][3] * pow(curT, 3) + controlArray.xparam[i][2] * pow(curT, 2) + controlArray.xparam[i][1] * curT + controlArray.xparam[i][0];
        double cy= controlArray.yparam[i][3] * pow(curT, 3) + controlArray.yparam[i][2] * pow(curT, 2) + controlArray.yparam[i][1] * curT + controlArray.yparam[i][0];
        _xs.push_back(cx);
        _ys.push_back(cy);
    }


    controlArray.xs[i].rdiff =  controlArray.xparam[i][1];
    controlArray.xs[i+1].ldiff = 3 * controlArray.xparam[i][3] * pow(t, 2) + 2 * controlArray.xparam[i][2] * t + controlArray.xparam[i][1];
    controlArray.ys[i].rdiff =  controlArray.yparam[i][1];
    controlArray.ys[i+1].ldiff = 3 * controlArray.yparam[i][3] * pow(t, 2) + 2 * controlArray.yparam[i][2] * t + controlArray.yparam[i][1];
    controlArray.drawPoints[i].xs.swap(_xs);
    controlArray.drawPoints[i].ys.swap(_ys);
}

void Backend::updateDrawArray()
{
    controlArray.pos.clear();
    drawArr.xs.clear();
    drawArr.ys.clear();
    for(int i=0;i<controlArray.drawSize();++i){
        drawArr.xs.insert(drawArr.xs.end(),controlArray.drawPoints[i].xs.begin(),controlArray.drawPoints[i].xs.end());
        drawArr.ys.insert(drawArr.ys.end(),controlArray.drawPoints[i].ys.begin(),controlArray.drawPoints[i].ys.end());
    }
    drawArr.size=drawArr.xs.size();
    for(int k=0;k<drawArr.size;++k){
        controlArray.pos.push_back(QPointF(drawArr.xs[k],drawArr.ys[k]));
    }
}

void Backend::updateCtrlPoints()
{
    controlArray.control.resize(controlArray.xs.size());
    controlArray.leftControl.resize(controlArray.xs.size());
    controlArray.rightControl.resize(controlArray.xs.size());
    for (int i = 0; i < controlArray.xs.size(); ++i) {
        controlArray.control[i].setX(controlArray.xs[i].val);
        controlArray.control[i].setY(controlArray.ys[i].val);
        controlArray.leftControl[i].setX(controlArray.xs[i].val - controlArray.xs[i].ldiff * controlArray.showpara);
        controlArray.leftControl[i].setY(controlArray.ys[i].val- controlArray.ys[i].ldiff *  controlArray.showpara);
        controlArray.rightControl[i].setX(controlArray.xs[i].val+ controlArray.xs[i].rdiff * controlArray.showpara);
        controlArray.rightControl[i].setY(controlArray.ys[i].val+ controlArray.ys[i].rdiff * controlArray.showpara);
    }
}
