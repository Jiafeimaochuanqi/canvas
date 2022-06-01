# 抛物线方程

![image-parabola](F:\coding\Qt\My\canvas\pic\parabola.png)

扫描线：$y=y_t$

基点：$(x_i,y_i)$

抛物线方程：
$$
\begin{aligned}
|y-y_t|&=\sqrt{(x-x_i)^2+(y-y_i)^2}\\
&\Leftrightarrow\\
(y-y_t)^2&=(x-x_i)^2+(y-y_i)^2\\
&\Leftrightarrow\\
2(y_i-y_t)y&=(x-x_i)^2+y_{i}^{2}-y_{t}^{2}\\
&\Leftrightarrow\\
2(y_i-y_t)(y-\frac{y_i+y_t}{2})&=(x-x_i)^2
\end{aligned}
$$
抛物线的最低点为$(x_i,\frac{y_i+y_t}{2})$，令$p=\frac{y_i+y_t}{2}-y_t,(x_i,\frac{y_i+y_t}{2})=(h,k)$，抛物线方程化简为
$$
4p(y-k)=(x-h)^{2}
$$



# 两个点的情况

![two](F:\coding\Qt\My\canvas\pic\two.png)

```c++
left->rightHalfEdge->twins=root->leftHalfEdge;
root->leftHalfEdge->twin = left->rightHalfEdge;
root->rightHalfEdge=root->leftHalfEdge;
right->leftHalfEdge=left->rightHalfEdge;
```



# 两条抛物线的交点

$$
\begin{aligned}
2(y_1-y_t)(y-\frac{y_1+y_t}{2})&=(x-x_1)^2\\
2(y_2-y_t)(y-\frac{y_2+y_t}{2})&=(x-x_2)^2\\
\frac{y_2-y_1}{2}&=\frac{(x-x_1)^2}{2(y_1-y_t)}-\frac{(x-x_2)^2}{2(y_2-y_t)}\\
d_1&=\frac{1}{2(y_1-y_t)}\\
d_2&=\frac{1}{2(y_2-y_t)}\\
a&=d_1-d_2\\
b&=2(x_2 *d_2 -x_1 * d_1)\\
c&=x_{1}^{2}*d_1-x_{2}^{2}*d_2-\frac{y_2-y_1}{2}\\
&=(x_{1}^{2}*d_1+\frac{y_1+y_t}{2})-(x_{2}^{2}*d_2+\frac{y_2+y_t}{2})\\
&=(x_{1}^{2}+y_{1}^{2}-y_{t}^{2})*d_{1}-(x_{2}^{2}+y_{2}^{2}-y_{t}^{2})*d_{2}
\end{aligned}
$$

### 三个点的情况

![three](F:\coding\Qt\My\canvas\pic\three.png)

```
addEvent(Arc* left, Arc* middle, Arc* right)
//y bottom of circle
```

![31](F:\coding\Qt\My\canvas\pic\31.png)

![32](F:\coding\Qt\My\canvas\pic\32.png)

![33](F:\coding\Qt\My\canvas\pic\33.png)
