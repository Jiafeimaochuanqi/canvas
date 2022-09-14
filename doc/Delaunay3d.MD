> [point inside sphere](https://github.com/lbc3402785/games102/tree/main/paper/Delaunay/predicates/cs372/cs372l12-Computing%20a%20Delaunay%20triangulation.MD)

# Normal to 4D hyper-plane

three vector  $ \overrightarrow{x},\overrightarrow{y},\overrightarrow{z}\in R^4$ to define the 4D hyperplane, we want to find anther vector to perpendicular these three vectors, denote it as $\overrightarrow{n}$, so we have equations as follows:
$$
\begin{aligned}
&\left\{ 
\begin{matrix}
\overrightarrow{n}\cdot \overrightarrow{n}=1\\
\overrightarrow{n}\cdot \overrightarrow{x}=0\\
\overrightarrow{n}\cdot \overrightarrow{y}=0\\
\overrightarrow{n}\cdot \overrightarrow{z}=0
\end{matrix}
\right.\\
&\Leftrightarrow\\
\left[
\begin{matrix}
n_{0}&n_{1}&n_{2}&n_{3}\\
x_{0}&x_{1}&x_{2}&x_{3}\\
y_{0}&y_{1}&y_{2}&y_{3}\\
z_{0}&z_{1}&z_{2}&z_{3}\\
\end{matrix}
\right]
\left[
\begin{matrix}
n_{0}\\
n_{1}\\
n_{2}\\
n_{3}\\
\end{matrix}
\right]&=\left[
\begin{matrix}
1\\
0\\
0\\
0\\
\end{matrix}
\right]\\

\end{aligned}
$$

$$
\begin{aligned}
n_{0}&=
\frac{
\left|
\begin{matrix}
1&\times&\times&\times\\
0&x_{1}&x_{2}&x_{3}\\
0&y_{1}&y_{2}&y_{3}\\
0&z_{1}&z_{2}&z_{3}\\
\end{matrix}
\right|
}
{
\left|
\begin{matrix}
n_{0}&n_{1}&n_{2}&n_{3}\\
x_{0}&x_{1}&x_{2}&x_{3}\\
y_{0}&y_{1}&y_{2}&y_{3}\\
z_{0}&z_{1}&z_{2}&z_{3}\\
\end{matrix}
\right|
}\\
n_{1}&=
\frac{
\left|
\begin{matrix}
\times&1&\times&\times\\
x_{0}&0&x_{2}&x_{3}\\
y_{0}&0&y_{2}&y_{3}\\
z_{0}&0&z_{2}&z_{3}\\
\end{matrix}
\right|
}
{
\left|
\begin{matrix}
n_{0}&n_{1}&n_{2}&n_{3}\\
x_{0}&x_{1}&x_{2}&x_{3}\\
y_{0}&y_{1}&y_{2}&y_{3}\\
z_{0}&z_{1}&z_{2}&z_{3}\\
\end{matrix}
\right|
}\\
n_{2}&=
\frac{
\left|
\begin{matrix}
\times&\times&1&\times\\
x_{0}&x_{1}&0&x_{3}\\
y_{0}&y_{1}&0&y_{3}\\
z_{0}&z_{1}&0&z_{3}\\
\end{matrix}
\right|
}
{
\left|
\begin{matrix}
n_{0}&n_{1}&n_{2}&n_{3}\\
x_{0}&x_{1}&x_{2}&x_{3}\\
y_{0}&y_{1}&y_{2}&y_{3}\\
z_{0}&z_{1}&z_{2}&z_{3}\\
\end{matrix}
\right|
}\\
n_{3}&=
\frac{
\left|
\begin{matrix}
\times&\times&\times&1\\
x_{0}&x_{1}&x_{2}&0\\
y_{0}&y_{1}&y_{2}&0\\
z_{0}&z_{1}&z_{2}&0\\
\end{matrix}
\right|
}
{
\left|
\begin{matrix}
n_{0}&n_{1}&n_{2}&n_{3}\\
x_{0}&x_{1}&x_{2}&x_{3}\\
y_{0}&y_{1}&y_{2}&y_{3}\\
z_{0}&z_{1}&z_{2}&z_{3}\\
\end{matrix}
\right|
}\\
\end{aligned}
$$



4D凸壳的投影必定是Delaunay 3d 三角化，理由如下：



1. 空间中共圆的四点或者不共面的四点必定共球。
2. 同一个4D超平面上与超抛物面相交的四点构成四边形，这四点在$oxyz$超平面的投影点必定共球。
3. 若另一点$\widehat{e}$在四边形$\hat{a}\hat{b}\hat{c}\hat{d}$上方，则$\widehat{e}$在$oxyz$平面的投影点$e$必在球($abcd$的外接球)的外面;若另一点$\widehat{e}$在四边形$\hat{a}\hat{b}\hat{c}\hat{d}$xia方，则$\widehat{e}$在$oxyz$平面的投影点$e$必在球($abcd$的外接球)的外面。
4. 4D凸壳的任意四边形面（法向朝上的除外），它与任意一个邻接面必然满足非邻接点都在邻接面上方，所以所有点的投影满足空球特性。