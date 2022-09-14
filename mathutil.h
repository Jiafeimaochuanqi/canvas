#ifndef MATHUTIL_H
#define MATHUTIL_H
#include <vector>
#include <Eigen/Dense>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
template <typename Type>
class MathUtil{
public:
    typedef typename  Eigen::Matrix<Type,2, 1> Vector2T;
    typedef typename  Eigen::Matrix<Type,Eigen::Dynamic, 1> VectorXT;
    typedef typename  Eigen::Matrix<Type,Eigen::Dynamic, Eigen::Dynamic> MatrixXT;
    /**
     * @brief InterpolationPolygon
     * @param in_pos:input
     * @param lb: start pos
     * @param rb: end pos
     * @param step:increment by step
     * @return
     */
    static std::vector<Vector2T> InterpolationPolygon(const std::vector<Vector2T> &in_pos, Type lb, Type rb, Type step) {
        std::vector<Vector2T> result;
        for(Type x=lb;x<rb;x+=step){
            Type y=0;
            for(int k=0;k<in_pos.size();k++){
                Type yk=in_pos[k].y();
                //calculate l_k(x)
                for(int i=0;i<in_pos.size();i++){
                    if(i!=k){
                        yk*=(x-in_pos[i].x())/(in_pos[k].x() - in_pos[i].x());;
                    }
                }
                y+=yk;
            }
            result.emplace_back(x,y);
        }
        return result;
    }
    static std::vector<Vector2T> ApproximationPolygon(const std::vector<Vector2T> &in_pos, int m, Type lb, Type rb, Type step,Type lambda=0.0f) {
        const int n = in_pos.size();
        m = std::min(m, std::max(n - 1, 0));
        MatrixXT B = LeastSquares(in_pos, m,lambda);
        std::vector<Eigen::Vector2f> result;
        for (Type x = lb; x <= rb; x += step) {
            Type y = 0, x_temp = 1.0f;
            for (int i = 0; i <= m; i++) {
                y += B(i, 0) * x_temp;
                x_temp *= x;
            }
            result.emplace_back(x, y);
        }
        return result;
    }
    static MatrixXT LeastSquares(const std::vector<Vector2T> &in_pos, int m, Type lambda = 0.0f){
        const int n = in_pos.size();
        Eigen::MatrixXf BTB(m + 1, m + 1);
        std::vector<float> pow_temp(n, 1.0f);
        for (int i = 0; i < 2 * m + 1; i++) {
            float sum = 0;
            for (int j = 0; j < n; j++) {
                sum += pow_temp[j];
                pow_temp[j] *= in_pos[j].x();
            }
            for (int j = 0; j <= i; j++) {
                if (j <= m && i - j <= m) {
                    BTB(j, i - j) = sum;
                }
            }
        }

        Eigen::MatrixXf norm = Eigen::MatrixXf::Identity(m + 1, m + 1);
        BTB += lambda * norm;

        Eigen::MatrixXf Y(m + 1, 1);
        std::fill(pow_temp.begin(), pow_temp.end(), 1.0f);
        for (int i = 0; i <= m; i++) {
            Y(i, 0) = 0.0f;
            for (int j = 0; j < n; j++) {
                Y(i, 0) += in_pos[j].y() * pow_temp[j];
                pow_temp[j] *= in_pos[j].x();
            }
        }

        //        Eigen::MatrixXf alpha = BTB.ldlt().solve(Y);
        Eigen::MatrixXf alpha ;
        if(lambda>0.0f)alpha= BTB.inverse()*Y;
        else {
            //alpha=BTB.ldlt().solve(Y);
            alpha=BTB.colPivHouseholderQr().solve(Y);
        }
        return alpha;
    }
    static std::vector<Vector2T> InterpolationGauss(const std::vector<Vector2T> &in_pos, Type sigma2, int m,Type lb, Type rb, Type step) {
        const int n = in_pos.size();
        m = std::min(m, std::max(n - 1, 0));

        Eigen::MatrixXf alpha = LeastSquares(in_pos, m);
        std::vector<float> y_approx(n);
        for (int i = 0; i < n; i++) {
            float y = 0, x_temp = 1.0f;
            for (int j = 0; j <= m; j++) {
                y += alpha(j, 0) * x_temp;
                x_temp *= in_pos[i].x();
            }
            y_approx[i] = y;
        }

        Eigen::MatrixXf A(n, n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A(i, j) = std::exp(-std::pow(in_pos[i].x() - in_pos[j].x(),2.0) / (2 * sigma2));
            }
        }

        Eigen::MatrixXf Y(n, 1);
        for (int i = 0; i < n; i++) {
            Y(i, 0) = in_pos[i].y() - y_approx[i];
        }

        Eigen::MatrixXf B = A.inverse() * Y;
        //        Eigen::MatrixXf B = A.ldlt().solve(Y);
        std::vector<Eigen::Vector2f> result;
        for (float x = lb; x <= rb; x += step) {
            float y = 0, x_temp = 1.0f;
            for (int i = 0; i <= m; i++) {
                y += alpha(i, 0) * x_temp;
                x_temp *= x;
            }
            for (int i = 0; i < n; i++) {
                y += B(i, 0) * std::exp(-std::pow(x - in_pos[i].x(),2) / (2 * sigma2));
            }
            result.emplace_back(x, y);
        }
        return result;
    }
    static std::vector<Type> ParameterizationUniform(const std::vector<Type> &pos_x, const std::vector<Type> &pos_y) {
        const size_t n = pos_x.size();
		if (n == 0)return {};
        if (n == 1) {
            return { 0.0f };
        }
        Type inv = 1.0f / (n - 1);
        std::vector<Type> result(n);
        for (size_t i = 0 ; i < n; i++) {
            result[i] = i * inv;
        }
        return result;
    }
    static std::vector<Type> ParameterizationChordal(const std::vector<Type> &pos_x, const std::vector<Type> &pos_y) {
        const size_t n = pos_x.size();
        if (n == 1) {
            return { 0.0f };
        }
        Type sum = 0.0f;
        std::vector<Type> result(n);
        std::vector<Type> dist(n - 1);
        for (size_t i = 1; i < n; i++) {
            dist[i - 1] = (Vector2T(pos_x[i - 1], pos_y[i - 1]) - Vector2T(pos_x[i], pos_y[i])).norm();
            sum += dist[i - 1];
        }
        result[0] = 0.0f;
        for (size_t i = 1; i < n - 1; i++) {
            result[i] = dist[i - 1] / sum;
            result[i] += result[i - 1];
        }
        result[n - 1] = 1.0f;
        return result;
    }
    static std::vector<Type> ParameterizationCentripetal(const std::vector<Type> &pos_x, const std::vector<Type> &pos_y) {
        const size_t n = pos_x.size();
        if (n == 1) {
            return { 0.0f };
        }
        Type sum = 0.0f;
        std::vector<Type> result(n);
        std::vector<Type> dist_sqrt(n - 1);
        for (size_t i = 1; i < n; i++) {
            Type dist = (Vector2T(pos_x[i - 1], pos_y[i - 1]) -Vector2T(pos_x[i], pos_y[i])).norm();
            dist_sqrt[i - 1] = std::sqrt(dist);
            sum += dist_sqrt[i - 1];
        }
        result[0] = 0.0f;
        for (size_t i = 1; i < n - 1; i++) {
            result[i] = dist_sqrt[i - 1] / sum;
            result[i] += result[i - 1];
        }
        result[n - 1] = 1.0f;
        return result;
    }
    static std::vector<Type> ParameterizationFoley(const std::vector<Type> &pos_x, const std::vector<Type> &pos_y) {
        const size_t n = pos_x.size();
        if (n == 1) {
            return { 0.0f };
        }
        std::vector<Type> dist(n + 1);
        for (size_t i = 1; i < n; i++) {
            dist[i] = (Eigen::Vector2f(pos_x[i - 1], pos_y[i - 1]) - Eigen::Vector2f(pos_x[i], pos_y[i])).norm();
        }
        dist[0] = dist[n] = 0.0f;
        std::vector<Type> angle(n);
        for (size_t i = 1; i < n - 1; i++) {
            Vector2T a = Vector2T(pos_x[i - 1], pos_y[i - 1]) - Vector2T(pos_x[i], pos_y[i]);
            Vector2T b = Vector2T(pos_x[i + 1], pos_y[i + 1]) - Vector2T(pos_x[i], pos_y[i]);
            angle[i] = a.dot(b) / dist[i] / dist[i + 1];
            angle[i] = std::min(M_PI - angle[i], M_PI / 2.0f);
        }
        angle[0] = angle[n - 1] = 0.0f;
        Type sum = 0.0f;
        std::vector<Type> diff(n - 1);
        for (size_t i = 0; i < n-1; i++) {
            diff[i] = dist[i+1] * (1.0f + 1.5f * (angle[i] * dist[i]) / (dist[i] + dist[i+1]) +
                    1.5f * (angle[i+1] * dist[i + 1]) / (dist[i+1] + dist[i + 2]));
            sum += diff[i];
        }

        std::vector<Type> result(n);
        result[0] = 0.0f;
        for (size_t i = 1; i < n - 1; i++) {
            result[i] = diff[i - 1] / sum;
            result[i] += result[i - 1];
        }
        result[n - 1] = 1.0f;
        return result;
    }
    /**
     * @brief Thomas
     * @param a: a_2,a_3,...,a_{n-1},a_n
     * @param b: b_1,b_2,...,b_{n-2},b_{n-1},b_{n}
     * @param c: c_1,c_2,...,c_{n-2},c_{n-1}
     * @param d: d_1,d_2,...,d_{n-2},d_{n-1},d_n
     * a_i-->a[i-2]
     * b_i-->b[i-1]
     * c_i-->c[i-1]
     * d_i-->d[i-1]
     * @return
     */
    static std::vector<Type> Thomas(const std::vector<Type>& a,const std::vector<Type>& b,const std::vector<Type>& c,const std::vector<Type>& d){
        size_t n=b.size();
        std::vector<Type> l(n-1,0);//l_2,l_3,...,l_{n-1},l_n
        std::vector<Type> u(n,0);//u_1,u_2,u_3,...,u_{n-1},u_n
        std::vector<Type> y(n,0);//y_1,y_2,y_3,...,y_{n-1},y_n
        //l_i-->l[i-2]
        //u_i-->u[i-1]
        u[0]=b[0];//u_1=b_1;
        for(int i=2;i<=n;i++){
            l[i-2]=a[i-2]/u[i-2];//l_i=a_i/u_{i-1}
            u[i-1]=b[i-1]-l[i-2]*c[i-2];//u_i=b_i-l_i*c_{i-1}
        }

        y[0]=d[0];//y_1=d_1
        //y_i-->y[i-1]
        for(int i=2;i<=n;++i){
            y[i-1]=d[i-1]-l[i-2]*y[i-2];//y_i=d_i － l_i*y_{i-1}
        }


        std::vector<Type> x(n,0);//x_1,x_2,...,x_n
        //x_i-->x[i-1]
        x[n-1]=y[n-1]/u[n-1];//x_n=y_n/u_n
        for(int i=n-1;i>=1;--i){
            x[i-1]=(y[i-1]-c[i-1]*x[i])/u[i-1];//x_i=(y_i-c_i*x_{i+1})/u_i
        }
        return x;
    }
    /**
     * @brief ThreeBlending2Order
     * @param x x_0,x_1,...,x_{n-1},x{n} in range [begin,end-1]
     * @param f f_0,f_1,...,f_{n-1},f{n} in range [begin,end-1]
     * @return
     */
    static std::vector<Type> ThreeBlending2Order(const std::vector<Type> &x, const std::vector<Type> &f,int begin,int end){
        int pointnum=end-begin;
        int n = pointnum-1;//三次曲线个数
        std::vector<Type> u(n+1,0);
        u[0]=2;
        std::vector<Type> v(n+1,0);
        v[0]=0;//v_0=0
        std::vector<Type> h(n);
        //h_0 h_1,...,h_{n-1}
        for (int i = 0; i < n ; i++) {
			//here x is global
            h[i] = x[begin+i + 1] - x[begin+i];
        }
        for (int i = 1; i <= n-1; i++) {
            u[i] =2*(h[i- 1] + h[i]) ;//u_i=2(h_{i-1}+h_i)
            v[i]=6*(f[begin+i+1]-f[begin+i])/h[i]-6*(f[begin+i]-f[begin+i-1])/h[i-1];//v_i=6*(f(x_i,x_{i+1})-f(x_{i-1},x_i))
        }
        u[n]=2;//2,u_1 u_2,...,u_{n-1},2
        v[n]=0;//0,v_1 v_2,...,v_{n-1},0
        std::vector<Type> a=h;
        a[n-1]=0;//h_0,h_1,...,h_{n-2},0
        std::vector<Type> c=h;
        c[0]=0;//0,h_1,...,h_{n-2},h_{n-1}
        std::vector<Type>M= Thomas(a,u,c,v);

        std::vector<float> param(4*n,0);//a+b(t-t_i)+c(t-t_i)^2+d(t-t_i)^3
        for (int i = 0; i < n; i++) {
			//here f is global
            param[4*i]=f[begin+i];
            param[4*i+1]=(f[begin + i + 1] - f[begin + i]) / h[i]  -(M[i]* h[i])/2.0f - (M[i + 1] - M[i]) * h[i] / 6.0f;
            param[4*i+2]=M[i]/2;
            param[4*i+3]=(M[i + 1] - M[i]) / (6.0 * h[i]);
        }
        return param;
    }
    static std::vector<Type> ThreeBlendingOneOrder(const std::vector<Type> &x, const std::vector<Type> &f,int begin,int end,Type dBegin,Type dEnd){
        int pointnum=end-begin;
        int n = pointnum-1;//三次曲线个数
        std::vector<Type> u(n+1,0);

        std::vector<Type> v(n+1,0);

        std::vector<Type> h(n);
        //h_0 h_1,...,h_{n-1}
        for (int i = 0; i < n ; i++) {
            h[i] = x[begin+i + 1] - x[begin+i];
        }
        u[0]=2*h[0];
        v[0]=6*((f[begin+1]-f[begin])/h[0]-dBegin);//v_0=6(f[x_0,x_1]-f_0')
        for (int i = 1; i <= n-1; i++) {
			//here f is global and h is local
            u[i] =2*(h[i- 1] + h[i]) ;//u_i=2(h_{i-1}+h_i)
            v[i]=6*(f[begin+i+1]-f[begin+i])/h[i]-6*(f[begin+i]-f[begin+i-1])/h[i-1];//v_i=6*(f(x_i,x_{i+1})-f(x_{i-1},x_i))
        }
        u[n]=2*h[n-1];//2h_0,u_1 u_2,...,u_{n-1},2h_{n-1}
        v[n]=6*(dEnd-(f[begin+n]-f[begin+n-1])/h[n-1]);//v_n=6(f_n'-f[x_{n-1},x_n]),here f is global and h is local
        std::vector<Type> M= Thomas(h,u,h,v);

        std::vector<float> param(4*n,0);//a+b(t-t_i)+c(t-t_i)^2+d(t-t_i)^3
        for (int i = 0; i < n; i++) {
			//here f is global and h is local
            param[4*i]=f[begin + i];
            param[4*i+1]=(f[begin + i + 1] - f[begin + i]) / h[i]  -(M[i]* h[i])/2.0f - (M[i + 1] - M[i]) * h[i] / 6.0f;
            param[4*i+2]=M[i]/2;
            param[4*i+3]=(M[i + 1] - M[i]) / (6.0 * h[i]);
        }
        return param;
    }
    static Eigen::Vector2f recursive_bezier(const std::vector<Eigen::Vector2f> &control_points, float t)
    {
        // TODO: Implement de Casteljau's algorithm
        int n=control_points.size()-1;
        std::vector<Eigen::Vector2f> f=control_points;
        for(int r=1;r<=n;++r){
            for(int i=0;i<=n-r;++i){
                f[i]=(1.0-t)*f[i]+t*f[i+1];
            }
        }
        return f[0];
    }
};
#endif // MATHUTIL_H
