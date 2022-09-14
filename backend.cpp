#include "backend.h"
#include <QVariantList>
#include <QDebug>

#include "mathutil.h"

constexpr float OFFSET = 1.0f;


Backend::Backend(QObject *parent): QObject(parent)
{
    fittingType=FittingType::Gauss;
    parameterizationType=ParameterizationType::Foley;
    curveType=CurveType::Bspline;
    change=false;
}

Backend::~Backend()
{
    using namespace HullDelaunayVoronoi::Hull;
    while(!animation.empty()){
        std::vector<Simplex<Vertex2>*> frame=animation.front();
        animation.pop();
        for(Simplex<Vertex2>* f:frame){
            for(Vertex2* v:f->Vertices()){
                delete v;
            }
            delete f;
        }
    }
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
QVariantList Backend::getOutput() const
{
    QVariantList l;
    l.reserve(output.size());
    for(QPointF p:output){
        l.append(p);
    }
    return l;
}

QVariantList Backend::getControl() const
{
    QVariantList l;
    l.reserve(controlArray.control.size());
    for(QPointF p:controlArray.control){
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
    int i=input.size();
    input.push_back(p);
    inputX.push_back(p.x());
    inputY.push_back(p.y());
    switch (curveType) {
    case Bspline:

        controlArray.addPoint(p);
        if (controlArray.nodeNum() >= 2) {
            controlArray.drawPoints.push_back(NodeArr());
            controlArray.xparam.push_back(std::vector<float>(4));
            controlArray.yparam.push_back(std::vector<float>(4));

        }
        break;
    case Bezier:
        switch (i%3) {
        case 0:
            controlArray.addPoint(p);
            if(i>0){
                controlArray.setLDiff(controlArray.nodeNum()-1,input[i-1]);
            }
            if (controlArray.nodeNum() >= 2) {
                controlArray.drawPoints.push_back(NodeArr());
            }
            break;
        case 1:
            controlArray.setRDiff(controlArray.nodeNum()-1,p);
            break;
        case 2:
            break;
        }
        break;
    }
    emit inputChanged(input);
}

void Backend::addPoint(QPointF p)
{
    //qDebug()<<p;
    points.append(p);
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
    switch (curveType) {
    case Bspline:
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
        break;
    case Bezier:
        for (int i = 0; i < input.size(); ++i) {
            double dist = sqrt(pow(p.x() - input[i].x(), 2) + pow(p.y() - input[i].y(), 2));
            if (dist < minv && dist < maxChoosedist) {
                minv = dist;
                res = i;
            }
        }
        break;
    }


    moveNodeNum=res;
    return res;
}

void Backend::setControl(QPointF p)
{

    if(moveNodeNum==-1)return;
    switch (curveType) {
    case Bspline:
        if (moveNodeNum < controlArray.nodeNum()){
            input[moveNodeNum]=p;
            inputX[moveNodeNum]=p.x();
            inputY[moveNodeNum]=p.y();
            controlArray.setPoint(moveNodeNum,p);
            if (moveNodeNum > 0) {
                calculateParamRange(moveNodeNum - 1);
                if (moveNodeNum < controlArray.nodeNum()-1) {
                    calculateParamRange(moveNodeNum);
                }
            }
            else if (moveNodeNum == 0) {
                calculateParamRange(0);
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
        break;
    case Bezier:
        input[moveNodeNum]=p;
        inputX[moveNodeNum]=p.x();
        inputY[moveNodeNum]=p.y();
        int nodeIndex;
        switch (moveNodeNum%3) {
        case 0:
            nodeIndex=moveNodeNum/3;
            controlArray.setPoint(nodeIndex,p);
            if(moveNodeNum>0){

                input[moveNodeNum-1].setX(controlArray.xs[nodeIndex].val - controlArray.xs[nodeIndex].ldiff * controlArray.showpara);
                input[moveNodeNum-1].setY(controlArray.ys[nodeIndex].val - controlArray.ys[nodeIndex].ldiff *  controlArray.showpara);
            }
            if(moveNodeNum<input.size()-1){
                input[moveNodeNum+1].setX(controlArray.xs[nodeIndex].val+ controlArray.xs[nodeIndex].rdiff * controlArray.showpara);
                input[moveNodeNum+1].setY(controlArray.ys[nodeIndex].val+ controlArray.ys[nodeIndex].rdiff * controlArray.showpara);
            }
            updateCtrlPoints();
            if (nodeIndex > 0) {
                calculateParamRange(nodeIndex - 1);
                if (nodeIndex < controlArray.nodeNum()-1) {
                    calculateParamRange(nodeIndex);
                }
            }
            else if (nodeIndex == 0) {
                calculateParamRange(0);
            }
            break;
        case 1:
            nodeIndex=moveNodeNum/3;
            controlArray.setRDiff(nodeIndex,p);
            updateCtrlPoints();
            if (nodeIndex < controlArray.fixed.size() - 1) {
                calculateParamRange(nodeIndex);
            }
            break;
        case 2:
            nodeIndex=moveNodeNum/3+1;
            if(nodeIndex<controlArray.nodeNum()){
                controlArray.setLDiff(nodeIndex,p);
                updateCtrlPoints();

                if (nodeIndex > 0) {
                    calculateParamRange(nodeIndex - 1);
                }
            }
            break;
        }
        break;
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
        calculateParamRange(controlArray.drawPoints.size() - 1);
    }
}

void Backend::bezier()
{
    updateCtrlPoints();
    if (controlArray.nodeNum() >= 2) {
        calculateParamRange(controlArray.drawPoints.size() - 1);
    }
}

void Backend::process()
{
    std::vector<Vector2> points;
    points.resize(this->points.size());
    for(int k=0;k<this->points.size();++k){
        points[k]=Vector2(this->points[k].x(),toTk(this->points[k].y()));
    }
    output.clear();
    // Construct diagram
    FortuneAlgorithm algorithm(points);
    auto start = std::chrono::steady_clock::now();
    algorithm.construct();
    auto duration = std::chrono::steady_clock::now() - start;
    std::cout << "construction: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';

    // Bound the diagram
    start = std::chrono::steady_clock::now();
    algorithm.bound(Box{-0.05, -0.05, (double)width+0.05, (double)height+0.05}); // Take the bounding box slightly bigger than the intersection box
    duration = std::chrono::steady_clock::now() - start;
    std::cout << "bounding: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
    VoronoiDiagram diagram = algorithm.getDiagram();

    // Intersect the diagram with a box
    start = std::chrono::steady_clock::now();
    bool valid = diagram.intersect(Box{0, 0, (double)width, (double)height});
    duration = std::chrono::steady_clock::now() - start;
    std::cout << "intersection: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
    if (!valid)
        throw std::runtime_error("An error occured in the box intersection algorithm");
    //one site relate to one face (which contains circular halfedge linked list)
    for (std::size_t i = 0; i < diagram.getNbSites(); ++i)
    {
        const VoronoiDiagram::Site* site = diagram.getSite(i);
        Vector2 center = site->point;
        VoronoiDiagram::Face* face = site->face;
        VoronoiDiagram::HalfEdge* halfEdge = face->outerComponent;
        if (halfEdge == nullptr)
            continue;
        /*while (halfEdge->prev != nullptr)
        {
            halfEdge = halfEdge->prev;
            if (halfEdge == face->outerComponent)
                break;
        }*/
        VoronoiDiagram::HalfEdge* start = halfEdge;
        while (halfEdge != nullptr)
        {
            if (halfEdge->origin != nullptr && halfEdge->destination != nullptr)
            {
                Vector2 origin = (halfEdge->origin->point - center) * OFFSET + center;
                Vector2 destination = (halfEdge->destination->point - center) * OFFSET + center;
                //drawEdge(window, origin, destination, sf::Color::Red);
                output.push_back(QPointF(origin.x,toTk(origin.y)));
                output.push_back(QPointF(destination.x,toTk(destination.y)));
            }
            halfEdge = halfEdge->next;
            if (halfEdge == start)
                break;
        }
    }

}

void Backend::quickHull2d()
{
    using namespace HullDelaunayVoronoi::Primitives;
    using namespace HullDelaunayVoronoi::Hull;
    while(!animation.empty()){
        std::vector<Simplex<Vertex2>*> frame=animation.front();
        animation.pop();
        for(Simplex<Vertex2>* f:frame){
            delete f->Vertices()[0];
            delete f->Vertices()[1];
            delete f;
        }
    }
    while(!centers.empty()){
        centers.pop();
    }
    if(points.size()<=1){
        return;
    }
    else if(points.size()==2){
        output.clear();
        output.push_back(points[0]);
        output.push_back(points[1]);
        return;
    }
    else if(points.size()==3){
        output.clear();
        output.push_back(points[0]);
        output.push_back(points[1]);
        output.push_back(points[0]);
        output.push_back(points[2]);
        output.push_back(points[1]);
        output.push_back(points[2]);
        return;
    }
    ConvexHull2* hull=new ConvexHull2();

    for(QPointF p:points){
        hull->AddVert(p.x(),p.y());
    }
    hull->compute();
    output.clear();
    for(Simplex<Vertex2>* f:hull->Simplexs()){
        QPointF p0(f->Vertices()[0]->X(),f->Vertices()[0]->Y());
        QPointF p1(f->Vertices()[1]->X(),f->Vertices()[1]->Y());
        output.push_back(p0);
        output.push_back(p1);
    }

    animation=hull->animation;
    centers=hull->centers;
    delete hull;
    std::cout << "========================" << std::endl;
}

void Backend::playHull2d()
{
    using namespace HullDelaunayVoronoi::Primitives;
    using namespace HullDelaunayVoronoi::Hull;
    if(animation.size()>0){
        std::vector<Simplex<Vertex2>*> frame=animation.front();
        animation.pop();
        output.clear();
        for(Simplex<Vertex2>* f:frame){
            QPointF p0(f->Vertices()[0]->X(),f->Vertices()[0]->Y());
            QPointF p1(f->Vertices()[1]->X(),f->Vertices()[1]->Y());
            output.push_back(p0);
            output.push_back(p1);
            for(Vertex2* v:f->Vertices()){
                delete v;
            }
            delete f;
        }

    }
    if(centers.size()>0){
        centroid=QPointF(centers.front()[0],centers.front()[1]);
        centers.pop();
    }
}

void Backend::delaunay2d()
{
    using namespace HullDelaunayVoronoi::Primitives;
    using namespace HullDelaunayVoronoi::Hull;
    using namespace HullDelaunayVoronoi::Delaunay;
    while(!animation.empty()){
        std::vector<Simplex<Vertex2>*> frame=animation.front();
        animation.pop();
        for(Simplex<Vertex2>* f:frame){
            delete f->Vertices()[0];
            delete f->Vertices()[1];
            delete f;
        }
    }
    while(!centers.empty()){
        centers.pop();
    }
    if(points.size()<=1){
        return;
    }
    else if(points.size()==2){
        output.clear();
        output.push_back(points[0]);
        output.push_back(points[1]);
        return;
    }
    else if(points.size()==3){
        output.clear();
        output.push_back(points[0]);
        output.push_back(points[1]);
        output.push_back(points[0]);
        output.push_back(points[2]);
        output.push_back(points[1]);
        output.push_back(points[2]);
        return;
    }

    DelaunayTriangulation2<Vertex2>* delaunay = new DelaunayTriangulation2<Vertex2>();
    for(QPointF p:points){
        delaunay->AddVert(p.x(),p.y());
    }

    delaunay->setCreateAnimation(true);
    delaunay->compute();
    animation=delaunay->Hull()->animation;
    centers=delaunay->Hull()->centers;
    output.clear();
    for(DelaunayCell<Vertex2>* cell : delaunay->Cells())
    {
        Simplex<Vertex2> *simplex= cell->Simplex();
        QPointF p0(simplex->Vertices()[0]->X(),simplex->Vertices()[0]->Y());
        QPointF p1(simplex->Vertices()[1]->X(),simplex->Vertices()[1]->Y());
        output.push_back(p0);
        output.push_back(p1);

        QPointF p2(simplex->Vertices()[2]->X(),simplex->Vertices()[2]->Y());
        output.push_back(p0);
        output.push_back(p2);

        output.push_back(p1);
        output.push_back(p2);
    }
    delete delaunay;
}

void Backend::playDelaunay2d()
{
    using namespace HullDelaunayVoronoi::Primitives;
    using namespace HullDelaunayVoronoi::Hull;
    using namespace HullDelaunayVoronoi::Delaunay;
    if(animation.size()>0){
        std::vector<Simplex<Vertex2>*> frame=animation.front();
        animation.pop();
        output.clear();
        for(Simplex<Vertex2>* simplex:frame){
            if(simplex->Normal()[2]<0){
                QPointF p0(simplex->Vertices()[0]->X(),simplex->Vertices()[0]->Y());
                QPointF p1(simplex->Vertices()[1]->X(),simplex->Vertices()[1]->Y());
                output.push_back(p0);
                output.push_back(p1);

                QPointF p2(simplex->Vertices()[2]->X(),simplex->Vertices()[2]->Y());
                output.push_back(p0);
                output.push_back(p2);

                output.push_back(p1);
                output.push_back(p2);
            }
            for(Vertex2* v:simplex->Vertices()){
                delete v;
            }
            delete simplex;
        }

    }
    if(centers.size()>0){
        centroid=QPointF(centers.front()[0],centers.front()[1]);
        centers.pop();
    }
}
bool Backend::InBound(const Vertex2& v)
{
    if (v.X() < -width || v.X() > width) return false;
    if (v.Y() < -height || v.Y() > height) return false;

    return true;
}
void Backend::voronoi2d()
{
    if(points.size()<3)return;
    using namespace HullDelaunayVoronoi::Primitives;
    using namespace HullDelaunayVoronoi::Hull;
    using namespace HullDelaunayVoronoi::Delaunay;
    using namespace HullDelaunayVoronoi::Voronoi;
    VoronoiMesh2<Vertex2>* voronoiMesh=new VoronoiMesh2<Vertex2>();
    for(QPointF p:points){
        voronoiMesh->AddVert(p.x(),p.y());
    }
    voronoiMesh->compute();
    output.clear();
    for(VoronoiRegion<Vertex2>* region : voronoiMesh->Regions())
    {
        bool draw = true;

//        for(DelaunayCell<Vertex2>* cell : region->Cells())
//        {
//            if (!InBound(cell->CircumCenter()))
//            {
//                draw = false;
//                break;
//            }
//        }

//        if (!draw) continue;

        for (VoronoiEdge<Vertex2> edge : region->Edges())
        {
            Vertex2 v0 = edge.From()->CircumCenter();
            Vertex2 v1 = edge.To()->CircumCenter();
            QPointF p0(v0.X(),v0.Y());
            QPointF p1(v1.X(),v1.Y());
            output.push_back(p0);
            output.push_back(p1);
        }
    }
    delete voronoiMesh;
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

const Backend::CurveType &Backend::getCurveType() const
{
    return curveType;
}

void Backend::setCurveType(const CurveType &newCurveType)
{
    curveType = newCurveType;
}

int Backend::getWidth() const
{
    return width;
}

void Backend::setWidth(int newWidth)
{
    qDebug()<<newWidth;
    this->width=newWidth;
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
    switch (curveType) {
    case Bspline:
        if (controlArray.xs[stpoint].fixed_diff == true||controlArray.xs[edpoint].fixed_diff == true) {
            rangeXParam=MathUtil<float>::ThreeBlendingOneOrder(ts,inputX,stpoint,edpoint+1,controlArray.xs[stpoint].rdiff,controlArray.xs[edpoint].ldiff);
            rangeYParam=MathUtil<float>::ThreeBlendingOneOrder(ts,inputY,stpoint,edpoint+1,controlArray.ys[stpoint].rdiff,controlArray.ys[edpoint].ldiff);
        }else{
            rangeXParam=MathUtil<float>::ThreeBlending2Order(ts,inputX,stpoint,edpoint+1);
            rangeYParam=MathUtil<float>::ThreeBlending2Order(ts,inputY,stpoint,edpoint+1);
        }
        for (int i = stpoint; i <= edpoint - 1; ++i) {
            int b = (i - stpoint) * 4;
            controlArray.xparam[i] ={rangeXParam[b], rangeXParam[b + 1], rangeXParam[b + 2], rangeXParam[b + 3]};
            controlArray.yparam[i] ={rangeYParam[b], rangeYParam[b + 1], rangeYParam[b + 2], rangeYParam[b + 3]};
            calculateRange(i);
        }
        break;
    case Bezier:
        for (int i = stpoint; i <= edpoint - 1; ++i) {
            Eigen::Vector2f p0(controlArray.control[i].x(),controlArray.control[i].y());
            Eigen::Vector2f p1(controlArray.rightControl[i].x(),controlArray.rightControl[i].y());
            Eigen::Vector2f p2(controlArray.leftControl[i+1].x(),controlArray.leftControl[i+1].y());
            Eigen::Vector2f p3(controlArray.control[i+1].x(),controlArray.control[i+1].y());
            std::vector<Eigen::Vector2f> controls={p0,p1,p2,p3};
            std::vector<double> _xs(controlArray.nodePerRange+1), _ys(controlArray.nodePerRange+1);
            for (int k = 0; k <= controlArray.nodePerRange; ++k) {
                float curT = ((float)k  / controlArray.nodePerRange);
                Eigen::Vector2f p=MathUtil<float>::recursive_bezier(controls,curT);
                _xs[k] =p(0);
                _ys[k] =p(1);
            }
            controlArray.drawPoints[i].xs.swap(_xs);
            controlArray.drawPoints[i].ys.swap(_ys);
        }
        break;
    }


    updateDrawArray();
    updateCtrlPoints();
}

void Backend::calculateRange(int i)
{
    double t=ts[i+1]-ts[i];
    std::vector<double> _xs(controlArray.nodePerRange+1), _ys(controlArray.nodePerRange+1);
    for (int k = 0; k <= controlArray.nodePerRange; ++k) {
        double curT = (k * t / controlArray.nodePerRange);
        _xs[k] = controlArray.xparam[i][3] * pow(curT, 3) + controlArray.xparam[i][2] * pow(curT, 2) + controlArray.xparam[i][1] * curT + controlArray.xparam[i][0];
        _ys[k] = controlArray.yparam[i][3] * pow(curT, 3) + controlArray.yparam[i][2] * pow(curT, 2) + controlArray.yparam[i][1] * curT + controlArray.yparam[i][0];
    }
    //    if(i==controlArray.nodeNum()-2){
    //        double curT = ts[i+1]-ts[i];
    //        double cx=controlArray.xparam[i][3] * pow(curT, 3) + controlArray.xparam[i][2] * pow(curT, 2) + controlArray.xparam[i][1] * curT + controlArray.xparam[i][0];
    //        double cy= controlArray.yparam[i][3] * pow(curT, 3) + controlArray.yparam[i][2] * pow(curT, 2) + controlArray.yparam[i][1] * curT + controlArray.yparam[i][0];
    //        _xs.push_back(cx);
    //        _ys.push_back(cy);
    //    }


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

int Backend::getHeight() const
{
    return height;
}

void Backend::setHeight(int newHeight)
{
    qDebug()<<newHeight;
    height = newHeight;
}

float Backend::toTk(float y)
{
    if(y==std::numeric_limits<float>::max()){
        return 0;
    }
    float result=height;
    if(y!=std::numeric_limits<float>::min()){
        result-=y;
    }
    return result;
}

QVariant Backend::getCentroid() const
{
    return QVariant(centroid);
}
