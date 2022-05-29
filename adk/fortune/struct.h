#ifndef STRUCT_H
#define STRUCT_H
#include <QVector>
#include <string>
#include <cmath>
namespace adk {
namespace fortune {

template <typename Point,typename Type=typename Point::value_type>
class VoronoiEdge{
public:
    Point left;
    Point right;
    Point *end;
    bool rightYFirst;
    bool rightXFirst;
    Type m;
    Type b;
    Type x;
    Point start;
    VoronoiEdge<Point>* partner;
public:
    VoronoiEdge(Point p,Point left,Point right):left(left),right(right){
        rightYFirst = right.y > left.y;
        rightXFirst = right.x < left.x;
        if (left.y == right.y){
            // vertical line is handled specially by declaring no y-intercept
            m = std::numeric_limits<Type>::max();
            b = NAN;//std::isnan(NAN)=true
            x = (right.x + left.x)/2;
            start  = Point(p.x, p.y);
        }
        else{
            //Compute line characteristics.
            m = (right.x - left.x) / (left.y - right.y);
            b = p.y - m*p.x;
            x = NAN;//std::isnan(NAN)=true
            start  = Point(p.x, p.y);
        }
    }
    void finish(int width,int height){
        Point p;
        if (rightYFirst){
            float y = width*m + b;
            if (y < 0){
                p=Point(-b/m, 0);
            }
            else if(y > height){
                p = Point((height - b)/m, height);
            }
            else{
                p = Point(width, y);
            }
        }
        else if (std::isnan(b)){
            p = (x, 0);
        }
        else{
            p = Point(0, b);
        }
        end =new Point(p);
    }
    /**
     * @brief Return point of intersection between two (half-)edges.
     * @param other
     * @return
     */
    Point* intersect(VoronoiEdge<Point>* other){
        Point* p;
        if (std::isnan(b)){
            if (std::isnan(other->b)){
                if (this->x == other->x){
                    return new Point(this->x,start.y);
                }else{
                    return nullptr;
                }
            }

            p = new Point(this->x, this->x*other->m + other->b);
        }
        else if (std::isnan(other->b)){
            p =new  Point(other->x, other->x*this->m + this->b);
        }
        else{
            //parallel lines have no intersection
            if (this->m == other->m){
                return nullptr;
            }
            else{
                Type x = (other->b -this->b)/(this->m - other->m);
                Type y = this->m*x + this->b;
                p = new Point(x,y);
            }
        }
        // self and other share a point. Ensure intersection is viable
        // based on orientation. Bisecting lines have -1/m slopes, which
        // is why X is paired with Y in the cX variables below.
        bool self_Xfirst = this->start.x < p->x;
        bool self_Yfirst = this->start.y < p->y;

        bool other_Xfirst = other->start.x < p->x;
        bool other_Yfirst = other->start.y < p->y;

        bool c1 = ! (self_Xfirst == this->rightYFirst);
        bool    c2 = ! (self_Yfirst == this->rightXFirst);
        bool   c3 = ! (other_Xfirst == other->rightYFirst);
        bool c4 = ! (other_Yfirst == other->rightXFirst);
        if (c1 || c2 || c3 || c4){
            return nullptr;
        }
        return p;
    }
    std::string toString(){
        std::string  endS ="";
        if (this->end){
            endS = end->toString();
        }
        return "[" + start.toString() + "," + endS + "]";
    }
};
template <typename Point,typename Type=typename Point::value_type>
class Event;
template <typename Point,typename Type=typename Point::value_type>
class Arc{
public:
    Arc<Point>* parent;
    Arc<Point>* left;
    Arc<Point>* right;
    bool isLeaf;
    Point* site;
    Event<Point>* circleEvent;
    VoronoiEdge<Point>* edge;
    Arc(Point* point=nullptr,VoronoiEdge<Point>* edge=nullptr):site(point),edge(edge){
        parent=nullptr;
        left=nullptr;
        right=nullptr;
        isLeaf = false;
        if (point){
            isLeaf = true;
        }
        circleEvent = nullptr;
    }
    std::string  toString(){
        std::string leftS = "";
        if (left){
            leftS = left->toString();
        }
        std::string rightS ="";
        if (right){
            rightS =right->toString();
        }

        return "(pt=" + site->toString() + ", left=" + leftS + ", right=" + rightS +")";
    }/**
     * @brief Given y-coordinate of sweep line and desired x intersection,return point on the bisection line with given x coordinate
     * @param x
     * @param sweepY
     * @return
     */
    Point pointOnBisectionLine(Type x,Type sweepY){
        if (site->y == sweepY){
            //vertical line halfway between x and site's x
            return Point((x+site->x)/2,sweepY);
        }
        else{
            // slope of bisection line is negative reciprocal
            // of line connecting points (x,sweepY) and self.site
            Type m = -(x - site->x)/(sweepY - site->y);
            Type halfway[2] = {(x+site->x)/2, (sweepY + site->y)/2};
            Type b = halfway[1] - m*halfway[0];
            Type y = m*x +b;
            return Point(x,y);
        }
    }
    void setLeft(Arc<Point>* n){
        left = n;
        n->parent = this;
    }
    void setRight(Arc<Point>* n){
        right = n;
        n->parent = this;
    }
    /**
     * @brief Find first ancestor with right link to a parent of self (if exists).
     * @param n
     */
    Arc<Point>* getLeftAncestor(){

        Arc<Point>* parent = this->parent;
        Arc<Point>*        n = this;
        while( parent != nullptr && parent->left == n){
            n = parent;
            parent = parent->parent;
        }

        return parent;
    }
    Arc<Point>* getRightAncestor(){

        Arc<Point>* parent = this->parent;
        Arc<Point>*        n = this;
        while( parent != nullptr && parent->right == n){
            n = parent;
            parent = parent->parent;
        }

        return parent;
    }
    /**
     * @brief Find largest value in left sub-tree.
     * @return
     */
    Arc<Point>* getLargestLeftDescendant(){

        Arc<Point>* n = left;
        while (n&&!n->isLeaf){
            n = n->right;
        }
        return n;
    }
    /**
     * @brief Find smallest value in right sub-tree.
     * @return
     */
    Arc<Point>* getSmallestRightDescendant(){

        Arc<Point>* n = right;
        while (n&&! n->isLeaf){
            n = n->left;
        }

        return n;
    }
    /**
     * @brief Remove leaf node from tree.
     */
    void remove(){
        Arc<Point>* grandParent = this->parent->parent;
        if (this->parent->left == this){
            if (grandParent->left == parent){
                grandParent->setLeft(parent->right);
            }
            else{
                grandParent->setRight(parent->right);
            }
        }
        else{
            if (grandParent->left == this->parent){
                grandParent->setLeft(this->parent->left);
            }
            else{
                grandParent->setRight(this->parent->left);
            }
        }
    }
};

template <typename Point,typename Type>
class Event{
public:
    Point* p;
    Point* site;
    Type y;
    bool deleted;
    Arc<Point> * node;//Circle events link back to Arc node
public:

    Event(Point* p,Point* site=nullptr):p(p),site(site){
        y = p->y;
        deleted=false;
    }
    //Overload the < operator.
    bool operator ==(const  Event<Point> &other){
        return (this->p->x == other->p->x && this->p->y == other->p->y);
    }
    bool operator !=(const  Event<Point> &other){
        return !(*this== other);
    }
    friend bool operator<= (const Event<Point>& e1, const Event<Point> &e2){
        return !(e1>e2);
    }
    friend bool operator>= (const Event<Point>& e1, const Event<Point> &e2){
        return !(e1<e2);
    }
    friend bool operator< (const Event<Point>& e1, const Event<Point> &e2)
    {
        if (e1.y > e2.y){
            return true;
        }else if(e1.y < e2.y){
            return false;
        }
        if(e1.p->x<e2.p->x){
            return  true;
        }
    }
    //Overload the > operator.
    friend bool operator> (const Event<Point>& e1, const Event<Point> &e2)
    {
        return e2<e1;
    }
};

template <typename Point>
class VoronoiPolygon{
public:
    QVector<Point> points;
    Point pt;
    Point* first;
    Point* last;
public:
    VoronoiPolygon(Point pt):pt(pt){
        first=last=nullptr;
    }
    bool isEmpty(){
        return first==nullptr;
    }
    void addToEnd(Point pt){

        if(points.size()==0){
            first=new Point(pt.x(),pt.y());
            last=new Point(pt.x(),pt.y());
            points.append(pt);
        }else{
            points.append(pt);
            delete last;
            last=new Point(pt.x(),pt.y());
        }
    }
    void addToFront(Point pt){

        if(points.size()==0){
            first=new Point(pt.x(),pt.y());
            last=new Point(pt.x(),pt.y());
            points.append(pt);
        }else{
            points.push_front(pt);
            delete first;
            first=new Point(pt.x(),pt.y());
        }
    }
    std::string toString(){
        std::string rep = "[";
        for(Point pt:points){

            rep = rep +"("+pt.x() + ","+pt.y()+"),";
            rep = rep + "]";
        }
        return rep;
    }

    friend std::ostream & operator<<(std::ostream& os,const VoronoiPolygon<Point>& polygon){
        os<<polygon.toString();
        return os;
    }
};
}
}
#endif // STRUCT_H
