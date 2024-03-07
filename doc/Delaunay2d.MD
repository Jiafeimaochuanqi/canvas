> [point inside sphere](https://github.com/lbc3402785/games102/tree/main/paper/Delaunay/predicates/cs372/cs372l12-Computing%20a%20Delaunay%20triangulation.MD)

3D凸壳的投影必定是Delaunay 2d 三角化，理由如下：

1. 同一个三角面的上的点在$oxy$平面的投影必定共圆。
2. 若另一点$\widehat{d}$在三角面$\hat{a}\hat{b}\hat{c}$上方，则$\widehat{d}$在$oxy$平面的投影点$d$必在圆的外面。
3. 3D凸壳的任意三角面（法向朝上的除外），它与任意一个邻接面必然满足非邻接点都在邻接面上方，所以所有点的投影满足空圆特性。

# DelaunayTriangulation

ObjectBuffer<VERTEX> *buffer;

一开始缓存所有的输入顶点指针

建立凸壳的过程逐渐把指针移交给凸壳

### InitiateFaceDatabase

4个初始点构建4个2-face，2-face 两两共享一条边建立邻接关系(点的对立面)