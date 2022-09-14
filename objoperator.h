#ifndef OBJOPERATOR_H
#define OBJOPERATOR_H
#include <vector>
#include <array>
#include <fstream>
template <typename T>
class ObjOperator
{
public:
    static void saveObjOutput(std::vector<T>& points,std::vector<size_t> faces,std::string name){
        std::ofstream out(name);
        for(int k=0;k<points.size()/3;k++){
            out<<"v "<<points[3*k+0]<<" "<<points[3*k+1]<<" "<<points[3*k+2]<<std::endl;
        }
        for(int k=0;k<faces.size()/3;k++){
            out<<"f "<<faces[3*k+0]+1<<" "<<faces[3*k+1]+1<<" "<<faces[3*k+2]+1<<std::endl;
        }
        out.close();
    }
    static void saveObjOutput(std::vector<T>& points,std::vector<std::array<size_t,3>> faces,std::string name){
        std::ofstream out(name);
        for(int k=0;k<points.size()/3;k++){
            out<<"v "<<points[3*k+0]<<" "<<points[3*k+1]<<" "<<points[3*k+2]<<std::endl;
        }
        for(int k=0;k<faces.size();k++){
            out<<"f "<<faces[k][0]+1<<" "<<faces[k][1]+1<<" "<<faces[k][2]+1<<std::endl;
        }
        out.close();
    }
    static void saveObjOutput(std::vector<std::array<T,3>>& points,std::vector<size_t> faces,std::string name){
        std::ofstream out(name);
        for(int k=0;k<points.size();k++){
            out<<"v "<<points[k][0]<<" "<<points[k][1]<<" "<<points[k][2]<<std::endl;
        }
        for(int k=0;k<faces.size()/3;k++){
            out<<"f "<<faces[3*k+0]+1<<" "<<faces[3*k+1]+1<<" "<<faces[3*k+2]+1<<std::endl;
        }
        out.close();
    }
    static void saveObjOutput(std::vector<std::array<T,3>>& points,std::vector<std::array<size_t,3>> faces,std::string name){
        std::ofstream out(name);
        for(int k=0;k<points.size();k++){
            out<<"v "<<points[k][0]<<" "<<points[k][1]<<" "<<points[k][2]<<std::endl;
        }
        for(int k=0;k<faces.size();k++){
            out<<"f "<<faces[k][0]+1<<" "<<faces[k][1]+1<<" "<<faces[k][2]+1<<std::endl;
        }
        out.close();
    }
};
#endif // OBJOPERATOR_H
