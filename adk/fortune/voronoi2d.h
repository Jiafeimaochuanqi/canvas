#ifndef VORONOI2D_H
#define VORONOI2D_H
#include "struct.h"
#include <QVector>
#include <queue>
namespace adk {
namespace fortune {
template <typename Type,typename Point=QPointF>
class PointT{
public:
    Type x;
    Type y;
    int idx;
    VoronoiPolygon<Point> *polygon;
public:
    typedef Type value_type;
    PointT(){
    }
    PointT(Type x,Type y,int idx=-1):x(x),y(y),idx(idx){

    }

    PointT(Point p,int idx=-1){
        x=p.x();
        y=p.y();
        this->idx=idx;
        polygon =new VoronoiPolygon<Point>(Point(x,y));

    }
    std::string toString(){
        return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }
};


template <typename Point,typename Type=float>
class Voronoi2D{
public:
    Voronoi2D(int width=800,int height=400):width(width),height(height)
    {
        firstPoint=nullptr;
    }
    void process(QVector<Point> points){
        pq=std::priority_queue<Event<PointT<Type>>, std::vector<Event<PointT<Type>>>,std::less< typename std::vector< Event<PointT<Type> > >::value_type> >();
        edges.clear();
        tree=nullptr;
        firstPoint=nullptr;
        stillOnFirstRow=true;
        this->points.clear();
        for(int i=0;i<points.size();++i){
            PointT<Type>* pt=new PointT<Type>(points[i],i);
            this->points.append(pt);
            Event<PointT<Type>> event(pt,pt);
            pq.push(event);
        }
        while(!pq.empty()){
            Event<PointT<Type>> event=pq.top();
            pq.pop();
            if(event.deleted){
                continue;
            }
            sweepPt = event.p;
            //Special case if multiple points are all on first row.
            if(stillOnFirstRow&& firstPoint!=nullptr){
                if (sweepPt->y != firstPoint->y){
                    stillOnFirstRow = false;
                }
            }
            if(event.site){
                processSite(event);
            }else{
                processCircle(event);
            }
        }
        //complete edges that remain and stretch to infinity
        if(tree&&tree->isLeaf){
            finishEdges(tree);
            //Complete Voronoi Edges with partners.
            for(VoronoiEdge<PointT<Type>>* e: edges){
                if (e->partner){
                    if (std::isnan(e->b)){
                        e->start.y =height;
                    }
                    else{
                        e->start = *(e->partner->end);
                    }
                }
            }
        }
    }
    /**
     * @brief Process a site event from the queue.
     * @param event
     */
    void processSite(Event<PointT<Type>>& event){
        if (tree == nullptr){
            this->tree =new Arc<PointT<Type>>(event.p);
            this->firstPoint = event.p;
            return;
        }
        // must handle special case when two points are at top-most y coordinate, in
        // which case the root is a leaf node. Note that when sorting events, ties
        // are broken by x coordinate, so the next point must be to the right
        if (this->tree->isLeaf && event.y == this->tree->site->y){
            Arc<PointT<Type>>* left = this->tree;
            Arc<PointT<Type>>* right = new Arc<PointT<Type>>(event.p);

            PointT<Type> start(Point((this->firstPoint->x + event.p->x)/2,this->height));
            VoronoiEdge<PointT<Type>> *edge =new VoronoiEdge<PointT<Type>>(start, *(this->firstPoint), *(event.p));

            this->tree =new Arc<PointT<Type>>(nullptr,edge);
            this->tree->setLeft(left);
            this->tree->setRight(right);

            this->edges.append(edge);
            return;
        }
        //find point on parabola where event.pt.x bisects with vertical line,
        Arc<PointT<Type>>* leaf = this->findArc(event.p->x);

        // Special case where there are multiple points, all horizontal with first point
        // so keep expanding to the right
        if(this->stillOnFirstRow){
            leaf->setLeft(new Arc<PointT<Type>>(leaf->site));
            PointT<Type> start (Point((leaf->site->x + event.p->x)/2, this->height));

            leaf->edge = new VoronoiEdge<PointT<Type>>(start, *(leaf->site), *(event.p));
            leaf->isLeaf = false;
            leaf->setRight(new Arc<PointT<Type>>(event.p));

            this->edges.append(leaf->edge);
            return;
        }
        // If leaf had a circle event, it is no longer valid
        // since it is being split
        if (leaf->circleEvent){
            leaf->circleEvent->deleted = true;
        }

        // Voronoi edges discovered between two sites. Leaf.site is higher
        // giving orientation to these edges.
        PointT<Type> start = leaf->pointOnBisectionLine (event.p->x, this->sweepPt->y);
        VoronoiEdge<PointT<Type>> *negRay =new VoronoiEdge<PointT<Type>>(start, *(leaf->site), *(event.p));
        VoronoiEdge<PointT<Type>> *posRay =new  VoronoiEdge<PointT<Type>>(start, *(event.p), *(leaf->site));
        negRay->partner = posRay;
        this->edges.append (negRay);

        // old leaf becomes root of two nodes, and grandparent of two
        leaf->edge = posRay;
        leaf->isLeaf = false;

        Arc<PointT<Type>> *left = new Arc<PointT<Type>>();
        left->edge = negRay;
        left->setLeft (new Arc<PointT<Type>>(leaf->site));
        left->setRight (new Arc<PointT<Type>>(event.p));

        leaf->setLeft (left);
        leaf->setRight (new Arc<PointT<Type>>(leaf->site));

        this->generateCircleEvent (left->left);
        this->generateCircleEvent (leaf->right);
    }
    /**
     * @brief Process circle event.
     * @param event
     */
    void processCircle(Event<PointT<Type>>& event){
        Arc<PointT<Type>> * node = event.node;
        // Find neighbor on the left and right.
        Arc<PointT<Type>> *leftA  = node->getLeftAncestor();
        Arc<PointT<Type>> *left   = leftA->getLargestLeftDescendant();
        Arc<PointT<Type>> *rightA = node->getRightAncestor();
        Arc<PointT<Type>> *right  = rightA->getSmallestRightDescendant();
        // Eliminate old circle events if they exist.
        if (left->circleEvent){
            left->circleEvent->deleted = true;
        }
        if (right->circleEvent){
            right->circleEvent->deleted = true;
        }
        //Circle defined by left - node - right. Terminate Voronoi rays
        PointT<Type> p = node->pointOnBisectionLine(event.p->x, this->sweepPt->y);

        //this is a real Voronoi point! Add to appropriate polygons
        if(left->site->polygon->last == node->site->polygon->first){
            node->site->polygon->addToEnd(Point(p.x,p.y));
        }
        else{
            node->site->polygon->addToFront(Point(p.x,p.y));
        }

        left->site->polygon->addToFront(Point(p.x,p.y));
        right->site->polygon->addToEnd(Point(p.x,p.y));

        //Found Voronoi vertex. Update edges appropriately
        leftA->edge->end = new PointT<Type>(p);
        rightA->edge->end = new PointT<Type>(p);
        // Find where to record new voronoi edge. Place with
        // (left) or (right), depending on which of leftA/rightA is higher
        // in the beachline tree. Without loss of generality, assume leftA is higher.
        // Reason is because node is being deleted and the highest ancestor (leftA) is
        // interior node that represents breakpoint involving node, and this interior
        // node must now represent the breakpoint [left|right]. Since leftA is higher,
        // it will remain while rightA is being removed (effectively replaced by right).
        Arc<PointT<Type>> *t = node;
        Arc<PointT<Type>> *ancestor = nullptr;
        while (t != this->tree){
            t = t->parent;
            if (t == leftA){
                ancestor = leftA;
            }
            else if (t == rightA){
                ancestor = rightA;
            }

            ancestor->edge = new VoronoiEdge<PointT<Type>> (p, *(left->site), *(right->site));
            this->edges.append (ancestor->edge);
        }

        // eliminate middle arc (leaf node) from beach line tree
        node->remove();

        // May find new neighbors after deletion so must check
        // for circles as well...
        this->generateCircleEvent(left);
        this->generateCircleEvent(right);
    }
    /**
     * @brief There is possibility of a circle event with this new node being the
     *  middle of three consecutive nodes. If so, then add new circle
     *  event to the priority queue for further processing.
     * @param node
     */
    void generateCircleEvent( Arc<PointT<Type>> *node){

        // Find neighbor on the left and right, should they exist.
        Arc<PointT<Type>> * leftA= node->getLeftAncestor();
        if (leftA==nullptr){
            return;
        }
        Arc<PointT<Type>> *left = leftA->getLargestLeftDescendant();

        Arc<PointT<Type>> *rightA = node->getRightAncestor();
        if (rightA=nullptr){
            return;
        }
        Arc<PointT<Type>> *right = rightA->getSmallestRightDescendant();

        // sanity check. Must be different
        if (left->site == right->site){
            return;
        }

        // If two edges have no intersection, leave now
        PointT<Type> *p = leftA->edge->intersect (rightA->edge);
        if (p ==nullptr){
            return;
        }

        Type radius = std::sqrt((p->x-left->site->x)*(p->x-left->site->x) + (p->y-left->site->y)*(p->y-left->site->y));

        // make sure choose point at bottom of circumcircle
        Event<PointT<Type>>* circleEvent = new Event<PointT<Type>>(new PointT<Type>(p->x, p->y-radius));
        if (circleEvent->p->y >= this->sweepPt->y){
            return;
        }

        node->circleEvent = circleEvent;
        circleEvent->node = node;
        pq.push(*circleEvent);
    }
    /**
     * @brief  Close all Voronoi edges against maximum bounding box, based on how edge extends.
     * @param tree
     */
    void finishEdges(Arc<PointT<Type>>* tree){
        tree->edge->finish(width, height);
        tree->edge->left->polygon.addToFront(tree->edge->end);
        tree->edge->right->polygon.addToEnd(tree->edge->end);

        if (! tree->left->isLeaf){
            finishEdges(tree->left);
        }
        if (! tree->right->isLeaf){
            finishEdges(tree->right);
        }
    }
    Arc<PointT<Type>>* findArc(Type x){
        Arc<PointT<Type>>* n=tree;
        while(n!=nullptr&&!n->isLeaf){
            Type lineX=computeBreakPoint(n);
            // if tie, can choose either one.
            if (lineX > x){
                n = n->left;
            }
            else{
                n = n->right;
            }
        }
        return n;
    }
    /**
     * @brief With sweep line Y coordinate and left/right children of interior node. You want
     *  to find the x-coordinate of the breakpoint, which changes based upon the y-value
     *  of the sweep line. Must compute intersection of two parabolas.
     *
     *  Parabola can be computed as 4p(y-k)=(x-h)^2 where (h,k) is the site point, which
     *  becomes the focal point for the parabola. p is the distance to the directrix
     *  (aka, the sweep line) from the site's point (site.y - sweepPt.y)
     *
     *  y1 = (1/4p1)x^2 + (-h1/2p1)x + (h1^2/4p1+k1) and compute for (h2,k2,p2)
     *
     *  Only subtlety is that to simplify equation, normalize y-coordinates so
     *  k1 = p1/4 and k2 = p2/4; seems to eliminate most errors.
     *
     *  Now set to each other and subtract to get:
     *
     *  0 = (1/4p1 - 1/4p2)x^2 + (-h1/2p1 + h2/2p2) + (h1^2/4p1+k1) - (h2^2/4p2+k2)
     *
     *  Compute for x using quadratic formula: (-b +/- sqrt(b^2-4ac))/2a
     * @param n
     * @return
     */
    Type computeBreakPoint(Arc<PointT<Type>>* n){
        Arc<PointT<Type>>* left = n->getLargestLeftDescendant();
        Arc<PointT<Type>>* right= n->getSmallestRightDescendant();

        //degenerate case: might be same point, so return it.
        if (left->site == right->site){
            return left->site->x;
        }
        //both on horizontal line? Decide based on relation to sweepPt.x
        Type p1 = left->site->y - this->sweepPt->y;
        Type p2 = right->site->y - this->sweepPt->y;
        if (p1 == 0 && p2 == 0){
            if (this->sweepPt->x > right->site->x){
                return right->site->x;
            }
            else if (this->sweepPt->x < left->site->x){
                return left->site->x;
            }
            else{
                //between, so can choose either one. Go right
                return right->site->x;
            }
        }
        //on same horizontal line as sweep. Break arbitrarily
        if (p1 == 0){
            return left->site->x;
        }
        if (p2 == 0){
            return right->site->x;
        }

        Type h1 = left->site->x;
        Type h2 = right->site->x;

        Type a = 1/(4*p1) - 1/(4*p2);
        Type b = -h1/(2*p1) + h2/(2*p2);
        Type c = (p1/4 + h1*h1/(4*p1)) - (p2/4 + h2*h2/(4*p2));

        //not quadratic. only one solution. What if b is zero?
        if (a == 0){
            Type x = -c/b;
            return x;
        }
        //two solutions, possibly
        Type sq = b*b - 4*a*c;

        Type x1 = (-b - std::sqrt(sq))/(2*a);
        Type x2 = (-b + std::sqrt(sq))/(2*a);

        //since left.site is to the left of right.site, base decision on respective heights
        if (left->site->y < right->site->y){
            return std::max(x1, x2);
        }
        return std::min(x1,x2);
    }
private:
    int width;
    int height;
    bool stillOnFirstRow;
    QVector<PointT<Type>*> points;
    QVector <VoronoiEdge<PointT<Type>>* > edges;
    PointT<Type>* sweepPt;
    PointT<Type>* firstPoint;
    Arc<PointT<Type>>* tree;
    std::priority_queue<Event<PointT<Type>>, std::vector<Event<PointT<Type>>>,std::less< typename std::vector< Event<PointT<Type> > >::value_type> > pq;
};
}
}
#endif // VORONOI2D_H
