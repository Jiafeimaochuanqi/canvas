# GAMES102 HW1

## 1. 拉格朗日插值 

$(n+1)$个点$(x_i,y_i),i=0,1,\cdots,n$插值得到$n$次多项式

设 $x_0<x_1<\cdots<x_n $为插值节点，若 $n$ 次多项式$l_k(x),k=0,1,\cdots,n$满足条件
$$
\begin{align}
l_k(x_i)=\delta_{ik}=\left\{ 
\begin{matrix}
1,i=k\\
0,i\neq k\\
\end{matrix}\right.\tag{1}
\end{align}
$$

$$
\begin{align}
l_k(x)=\prod\limits_{i=0,i\neq k}^{n}{\frac{x-x_i}{x_k-x_i}},k=0,1,\cdots,n\tag{2}
\end{align}
$$




成为Lagrange插值基函数。

引入记号
$$
\begin{align}

w_{n+1}(x)=\prod\limits_{i=0}^{n}{(x-x_i)}\tag{3}

\end{align}
$$
容易求得
$$
\begin{align}

w_{n+1}{’}(x_k)=\prod\limits_{i=0,i\neq k}^{n}{(x_k-x_i)}

\end{align}
$$
于是,$(2)$式可以写成
$$
\begin{align}

l_k(x)=\frac{w_{n+1}(x)}{(x-x_k)w_{n+1}{’}(x_k)}

\end{align}
$$


值得注意的是,插值基函数仅由节点确定,与被插函数无关 。

由$(1)$式易见,满足插值条件
$$
\begin{align}

L_n(x_i)=y_i,i=0,1,\cdots,n

\end{align}
$$
的插值多项式$L_n(x)$为
$$
\begin{align}

L_n(x)=\sum\limits_{k=0}^{n}{y_k l_k(x)}

\end{align}
$$
称之为 Lagrange 插值多项式

## 2. 最小二乘拟合

当数据点个数较多时，插值会导致多项式曲线阶数过高，带来不稳定因素。可通过固定幂基函数的最高次数 $m(m < n)$。设$f(x)=\sum\nolimits_{i=0}^{m}{\alpha_{i}B_{i}(x)},B_{i}(x)=x^i$。将$n+1$个数据点代入：
$$
\begin{align}
\left\{
\begin{matrix}
y_0=\alpha_0+\alpha_1 x_0+\alpha_1 x_{0}^2+\cdots+\alpha_m x_{0}^m\\
y_1=\alpha_0+\alpha_1 x_1+\alpha_1 x_{1}^2+\cdots+\alpha_m x_{1}^m\\
\vdots\\
y_n=\alpha_0+\alpha_1 x_n+\alpha_1 x_{n}^2+\cdots+\alpha_m x_{n}^m\\
\end{matrix}\right.
\end{align}\tag{4}
$$
注意到在在公式 $(4)$ 中，方程的个数多余未知数个数，导致无精确解。使用最小二乘法可以
使得函数曲线尽可能靠近数据点，即
$$
\begin{align}
\underset{\alpha}{\min}{\sum\limits_{i=0}^{n}{(y_i-\sum\limits_{i=0}^{m}{\alpha_{i}x^{i}}})^2}
\end{align}\tag{5}
$$
将$(5)$ 式改写为矩阵形式，令
$$
Y=
\left[
\begin{matrix}
y_0\\
y_1\\
\vdots\\
y_n
\end{matrix}
\right],
B=
\left[
\begin{matrix}
1&x_0&x_0^2&\cdots&x_0^m\\
1&x_1&x_1^2&\cdots&x_1^m\\
\vdots\\
1&x_n&x_n^2&\cdots&x_n^m
\end{matrix}
\right],
\alpha=
\left[
\begin{matrix}
\alpha_0\\
\alpha_1\\
\vdots\\
\alpha_m
\end{matrix}
\right]
$$
$(5)$ 式等价于
$$
\underset{\alpha}{\min}{||Y-B\alpha||_2^2}\\
\Leftrightarrow\\
B^TY=B^TB\alpha
$$
通过求解方程$B^TY=B^TB\alpha$即可得到最小二乘拟合这组点的 m 次多项式曲线
$$
f(x)=\alpha_0+\alpha_0 x+\alpha_1 x^2+\cdots+\alpha_m x^m
$$

$$
B^TB=\left[ \begin{matrix}
   n & \sum\limits_{i=1}^{n}{{{x}_{i}}} & \sum\limits_{i=1}^{n}{x_{i}^{2}} & \sum\limits_{i=1}^{n}{x_{i}^{3}} & {} & \sum\limits_{i=1}^{n}{x_{i}^{m}}  \\
   \sum\limits_{i=1}^{n}{{{x}_{i}}} & \sum\limits_{i=1}^{n}{x_{i}^{2}} & \sum\limits_{i=1}^{n}{x_{i}^{3}} & {} & \sum\limits_{i=1}^{n}{x_{i}^{m}} & \sum\limits_{i=1}^{n}{x_{i}^{m+1}}  \\
   \sum\limits_{i=1}^{n}{x_{i}^{2}} & \sum\limits_{i=1}^{n}{x_{i}^{3}} & {} &  & \sum\limits_{i=1}^{n}{x_{i}^{m+1}} & \sum\limits_{i=1}^{n}{x_{i}^{m+2}}  \\
   \sum\limits_{i=1}^{n}{x_{i}^{3}} & {} &  &  &  & \vdots   \\
   \vdots  &  &  &  & \cdots  & \sum\limits_{i=1}^{n}{x_{i}^{2m-1}}  \\
   \sum\limits_{i=1}^{n}{x_{i}^{m}} & \sum\limits_{i=1}^{n}{x_{i}^{m+1}} & \sum\limits_{i=1}^{n}{x_{i}^{m+2}} & \cdots  & \sum\limits_{i=1}^{n}{x_{i}^{2m-1}} & \sum\limits_{i=1}^{n}{x_{i}^{2m}}  \\
\end{matrix} \right]
$$

$$
B^TY=\left( \begin{matrix}
   \sum\limits_{i=1}^{n}{{{y}_{i}}}  \\
   \sum\limits_{i=1}^{n}{{{x}_{i}}{{y}_{i}}}  \\
   \sum\limits_{i=1}^{n}{x_{i}^{2}{{y}_{i}}}  \\
   \sum\limits_{i=1}^{n}{x_{i}^{m}{{y}_{i}}}  \\
\end{matrix} \right)
$$

## 3. 岭回归拟合



当$B^TB$ 接近于奇异时，$(B^TB)−1$ 有较大误差，拟合结果不稳定。为此在最小二乘的误差函数中添加正则项，即
$$
\begin{align}
\underset{\alpha}{\min}{\sum\limits_{i=0}^{n}{(y_i-\sum\limits_{i=0}^{m}{\alpha_{i}x^{i}}})^2}
\end{align}+\lambda\sum\limits_{i=0}^{m}{\alpha_i^2}\tag{6}
$$

将$(6)$ 式改写为矩阵形式等价于
$$
\underset{\alpha}{\min}{||Y-B\alpha||_2^2}+\lambda\alpha^T\alpha\\
\Leftrightarrow\\
B^TY=(B^TB+\lambda I)\alpha\\
\Rightarrow\\
\alpha=(B^TB+\lambda I)^{-1}B^TY
$$
同样可得到最小二乘拟合这组点的 m 次多项式曲线
$$
f(x)=\alpha_0+\alpha_0 x+\alpha_1 x^2+\cdots+\alpha_m x^m
$$

## 4. 高斯基函数插值

设$f(x)=a+\sum\nolimits_{i=0}^{n}b_i g_i(x)$，其中
$$
b_i(x)=e^{-(x-x_i)^2/(2\sigma^2)}
$$

将 $n + 1$ 个数据点代入：
$$
\left\{
\begin{matrix}
y_0=a+b_0 g_0(x_0)+b_1 g_1(x_0)+\cdots++b_n g_n(x_0)\\
y_1=a+b_0 g_0(x_1)+b_1 g_1(x_1)+\cdots++b_n g_n(x_1)\\
\vdots\\
y_n=a+b_0 g_0(x_n)+b_1 g_1(x_n)+\cdots++b_n g_n(x_n)\\
\end{matrix}
\right.
\tag{7}
$$
可以发现公式 $(7)$中，未知数个数多余方程个数，方程有多个解，为此可以添加约束条件。我们将通过最小二乘拟合得到的结果作为$a$，再将残差做$n$个参数的高斯基函数拟合

令
$$
Y=
\left[
\begin{matrix}
y_0-\sum\limits_{i=0}^{m}{\alpha_{i}x_0^{i}}\\
y_1-\sum\limits_{i=0}^{m}{\alpha_{i}x_1^{i}}\\
\vdots\\
y_n-\sum\limits_{i=0}^{m}{\alpha_{i}x_n^{i}}
\end{matrix}
\right],
G=
\left[
\begin{matrix}
g_0(x_0)&g_1(x_0)&\cdots&g_n(x_0)\\
g_0(x_1)&g_1(x_1)&\cdots&g_n(x_1)\\
\vdots\\
g_0(x_n)&g_1(x_n)&\cdots&g_n(x_n)
\end{matrix}
\right],
b=
\left[
\begin{matrix}
b_0\\
b_1\\
\vdots\\
b_n
\end{matrix}
\right]
$$
即：通过求解关于 $\alpha$的线性方程组$ Y = Gb $即可得到插值这组点的径向基函数插值函数
$$
f(x)=a+\sum\limits_{i=0}^{n}b_i g_i(x)
$$
由于 $x_i$ 的值较大，故我们取 $\sigma = 50$

