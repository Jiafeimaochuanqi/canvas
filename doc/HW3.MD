# GAMES102 HW3

## 1. 任务

使用单参数曲线来拟合平面上任意有序点列

## 2. 方法

$(n+1)$个点$(x_i,y_i),i=0,1,\cdots,n$的$x_i,y_i$分别作为函数值拟合两个函数，参数为$t_i,i=0,1,\cdots,t_n$，拟合函数
$$
\left[\begin{matrix}x\\y\\\end{matrix}\right]=\left[\begin{matrix}f(t)\\g(t)\\\end{matrix}\right]
$$
选用作业1的拟合方法得到两个函数。

## 3. 参数化

### Uniform Parametrization

- $t_{i+1}-t_{i}=C$

### Chordal Parametrization

- $t_{i+1}-t_{i}=\lVert \bold{P}_{i+1}-\bold{P}_{i} \rVert$

### Centripetal Parametrization

- $t_{i+1}-t_{i}=\sqrt{\lVert \bold{P}_{i+1}-\bold{P}_{i} \rVert}$

### Foley Parametrization

- $$
  t_{i+1}-t_{i}=\left\{
             \begin{array}{lr}
             d_{i}*(1 + \frac{3}{2}\frac{\hat{\alpha}_{i+1}d_{i}}{d_{i}+d_{i+1}}), & i=0 \\
             d_{i}*(1 + \frac{3}{2}\frac{\hat{\alpha}_{i}d_{i-1}}{d_{i-1}+d_{i}}), & i+1=n-1.\\
             d_{i}*(1 + \frac{3}{2}\frac{\hat{\alpha}_{i}d_{i-1}}{d_{i-1}+d_{i}} + \frac{3}{2}\frac{\hat{\alpha}_{i+1}d_{i}}{d_{i}+d_{i+1}}), & else
             \end{array}
  \right.
  $$

  $\hat{\alpha}_i=\min{(\frac{\pi}{2},\pi-\alpha_i)}$,
  $\alpha_i=angle(\bold{P}_{i-1},\bold{P}_{i},\bold{P}_{i+1})$

  $d_i=\lVert \bold{P}_{i+1}-\bold{P}_{i} \rVert$.

## 4. 三次样条插值

设在区间$[a , b ]$上取 $n ＋１$ 个节点 $a ＝ x_0<x_1 <\cdots< x_n ＝ b$ ,给定这些点的函数值 $f (x_i ) ＝ f_i(i=0,1,\cdots,n) $。 若 $S ( x )$满足条件：

(1) $S(x)\in C^2[a,b]$

(2) $S(x_i)=f_i,i=0,1,\cdots,n$

(3) 在每个小区间$[x_i,x_{i+1}]$上，$S(x)$是一个3次多项式，则称 $S (x )$为 $f ( x )$在$[ a , b ]$上的 ３ 次样条插值函数 。

３ 次样条插值函数是分段 ３ 次多项式,在每个小区间$[ x_i , x_i+1]$上可以写成
$$
S(x)=a_i x^3+b_ix^2+c_ix+d_i,i=0,1,\cdots,n
$$
其中 $a_i,b_i,c_i$ 和 $d_i$ 为待定系数 。 所以, $S ( x )$共有 $4n$ 个待定参数 。 根据 $S ( x )$在$[ a , b ]$上 ２ 阶导数连续的条件,在节点 $x_i (i＝1,2,\cdots,n-1)$处应满足连续性条件
$$
S^{(k)}(x_i-0)=S^{(k)}(x_i+0),k=0,1,2.
$$
共有 $３(n-1)$个条件 。 再加上$n+1$ 个插值条件,共有 $4n-2$个条件 ． 因此,还需
要两个条件才能确定 $S(x)$ 。

### 4.1 三弯矩算法

根据推导得到方程：
$$
\left(\begin{matrix}
	2&\lambda_0&&&\\
	\mu_1&2&\lambda_1&&\\
	&\ddots&\ddots&\ddots&\\
	&&\mu_{n-1}&2&\lambda_{n-1}\\
	&&&\mu_n&2	
\end{matrix}\right)
\left(
\begin{matrix}
M_0\\
M_1\\
\vdots\\
M_{n-1}\\
M_{n}\\
\end{matrix}
\right)
=
\left(
\begin{matrix}
d_0\\
d_1\\
\vdots\\
d_{n-1}\\
d_{n}\\
\end{matrix}
\right)\tag{1}
$$
其中


$$
h_i=x_{i+1}-x_i,i=0,1,\cdots,n-1\\
f[x_i,x_{i+1}]=\frac{f(x_i)-f(x_{i+1})}{x_i-x_{i+1}},i=0,1,\cdots,n-1\\
f[x_i,x_{i+1},x_{i+2}]=\frac{f[x_i,x_{i+1}]-f[x_{i+1},x_{i+2}]}{x_i-x_{i+2}}=\frac{f[x_{i+1},x_{i+2}]-f[x_i,x_{i+1}]}{(h_{i}+h_{i+1})}\\
\mu_i=\frac{h_{i-1}}{h_{i-1}+h_{i}},\lambda_i=\frac{h_{i}}{h_{i-1}+h_{i}}=1-\mu_i,i=1,2,\cdots,n-1\\
d_i=6f[x_{i-1},x_i,x_{i+1}]=6\frac{f[x_{i},x_{i+1}]-f[x_{i-1},x_{i}]}{(h_{i-1}+h_{i})},i=1,2,\cdots,n-1
$$
左边的矩阵$\lambda_0,\mu_n$由于缺少两个条件未知。

#### 4.1.1 二次导数边界

取区间的两个端点的二次导数为0，即$M_0=M_n=0$，得到
$$
\lambda_0=\mu_n=0,d_0=2f''_0,d_n=2f''_n=0
$$
因此$(1)$式简化为
$$
\left(\begin{matrix}
	2&0&&&\\
	\mu_1&2&\lambda_1&&&\\
	&\ddots&\ddots&\ddots\\
	&&\mu_{n-1}&2&\lambda_{n-1}\\
	&&&0&2\\
\end{matrix}\right)
\left(
\begin{matrix}
M_0\\
M_1\\
\vdots\\
M_{n-1}\\
M_{n}\\
\end{matrix}
\right)
=
\left(
\begin{matrix}
d_0\\
d_1\\
\vdots\\
d_{n-1}\\
d_{n}\\
\end{matrix}
\right)\tag{2}
$$
或者等式左右两边将第$i$行乘以$(h_{i-1}+h_{i})$得到
$$
\left(\begin{matrix}
	2&0&&&\\
	h_0&u_1&h_1&&&\\
	&\ddots&\ddots&\ddots\\
	&&h_{n-2}&u_{n-1}&h_{n-1}\\
	&&&0&2\\
\end{matrix}\right)
\left(
\begin{matrix}
M_0\\
M_1\\
\vdots\\
M_{n-1}\\
M_{n}\\
\end{matrix}
\right)
=
\left(
\begin{matrix}
v_0\\
v_1\\
\vdots\\
v_{n-1}\\
v_{n}\\
\end{matrix}
\right)\tag{3}
$$
其中
$$
u_i=2(h_{i-1}+h_{i}),i=1,2,\cdots,n-1\\
v_i=d_i(h_{i-1}+h_{i})=6(f[x_i,x_{i+1}]-f[x_{i-1},x_{i}])\\
v_0=d_0=0\\
v_n=d_n=0
$$

求出所有二阶导数值之后位于区间$[x_i,x_{i+1}],i=0,1,2,\cdots,n-1$的点使用以下公式进行插值
$$
\begin{align}
S_i(x)&=f(x_i)+[-\frac{M_ih_i}{2}+\frac{f(x_{i+1})-f(x_i)}{h_i}-\frac{(M_{i+1}-M_i)h_i}{6}](x-x_i)\\
&+\frac{M_i}{2}(x-x_i)^2+(\frac{M_{i+1}-M_i}{6h_i})(x-x_i)^3
\end{align}
$$

#### 4.1.2 一次导数边界

取区间的两个端点的一阶导数为已知值，令
$$
\lambda_0=\mu_n=1,d_0=\frac{6}{h_0}(f(x_0,x_1)-f_0'),d_n=\frac{6}{h_{n-1}}(f_n'-f[x_{n-1},x_n])
$$
因此$(1)$式简化为
$$
\left(\begin{matrix}
	2&1&&&\\
	\mu_1&2&\lambda_1&&&\\
	&\ddots&\ddots&\ddots\\
	&&\mu_{n-1}&2&\lambda_{n-1}\\
	&&&1&2\\
\end{matrix}\right)
\left(
\begin{matrix}
M_0\\
M_1\\
\vdots\\
M_{n-1}\\
M_{n}\\
\end{matrix}
\right)
=
\left(
\begin{matrix}
d_0\\
d_1\\
\vdots\\
d_{n-1}\\
d_{n}\\
\end{matrix}
\right)\tag{4}
$$
或者等式左右两边将第$i,i=1,2,\cdots,n-1$行乘以$(h_{i-1}+h_{i})$，第0行乘以$h_0$，第$n$行乘以$h_{n-1}$得到
$$
\left(\begin{matrix}
	2h_0&h_0&&&\\
	h_0&u_1&h_1&&&\\
	&\ddots&\ddots&\ddots\\
	&&h_{n-2}&u_{n-1}&h_{n-1}\\
	&&&h_{n-1}&2h_{n-1}\\
\end{matrix}\right)
\left(
\begin{matrix}
M_0\\
M_1\\
\vdots\\
M_{n-1}\\
M_{n}\\
\end{matrix}
\right)
=
\left(
\begin{matrix}
v_0\\
v_1\\
\vdots\\
v_{n-1}\\
v_{n}\\
\end{matrix}
\right)\tag{5}
$$
其中
$$
v_0=d_0*h_0=6(f(x_0,x_1)-f_0')\\
v_n=d_n*h_{n-1}=6(f_n'-f[x_{n-1},x_n])
$$
其他定义同方程(3)

**可见方程$(5)$同样满足追赶法的要求**


### 4.2 追赶法

设有方程组 $Ax ＝ d$ ,其中 $d =(d_1,d_2,\cdots,d_n)^T$ ,系数矩阵 $A$ 是三对角形矩阵
$$
A=
\left(
\begin{matrix}
b_1&c_1&&&\\
a_2&b_2&c_2&&\\
&\ddots&\ddots&\ddots&\\
&&a_{n-1}&b_{n-1}&c_{n-1}\\
&&&a_n&b_n\\
\end{matrix}
\right)
$$
且满足
$$
|b_1|>|c_1|\ge 0,|b_n|>|a_n|\ge 0\\
|b_i|\ge |a_i|+|c_i|,a_ic_i\neq 0,i=2,3,\cdots,n-1
$$
则 $A$ 非奇异，可以进行LU分解， 并且，$L$ 和$U$ 有如下形式
$$
L=
\left(
\begin{matrix}
1&&&&\\
l_2&1&&&\\
&l_3&1&&\\
&&\ddots&\ddots&\\
&&&l_n&1
\end{matrix}
\right),
U=
\left(
\begin{matrix}
u_1&c_1&&&\\
&u_2&c_2&&\\
&&\ddots&\ddots&\\
&&&\ddots&c_{n-1}\\
&&&&u_n
\end{matrix}
\right)
$$
利用$A=LU$可得
$$
\left\{
\begin{matrix}
\begin{align}
u_1&=b_1\\
l_i&=\frac{a_i}{u_{i-1}},i=2,3,\cdots,n,\\
u_i&=b_i-l_ic_{i-1},i=2,3,\cdots,n.
\end{align}
\end{matrix}
\right.
$$
由此可求得 L 和 U 的所有元素 ． 解原方程组 $Ax=d $可分为两步,即求解$Ly=d$和$ Ux=y $,计算公式为
$$
\begin{split}
&\left\{
\begin{matrix}
\begin{split}
y_1&=d_1,\\
y_i&=d_i-l_iy_{i-1},i=2,3,\cdots,n.
\end{split}
\end{matrix}
\right.\\
&\left\{
\begin{matrix}
\begin{split}
x_n&=\frac{y_n}{u_n},\\
x_i&=\frac{y_i-c_ix_{i+1}}{u_i},i=n-1,n-2,\cdots,1.
\end{split}
\end{matrix}
\right.
\end{split}
$$



