#include "test.h"
#define _USE_MATH_DEFINES
#include "geometry/Hull/ConvexHull.h"
#include "geometry/Voronoi/VoronoiMesh3.h"
#include <QDir>
#include <QFileInfo>
#include <fstream>
#include <list>

#include <math.h>
#include "tiny_obj_loader.h"
#include "objoperator.h"
typedef std::vector<tinyobj::shape_t> Shape;
typedef std::vector<tinyobj::material_t> Material;
typedef tinyobj::attrib_t Attribute;
Test::Test()
{

}


void Test::testConvexHull3D()
{
    using namespace MyCGAL::Primitives;
    using namespace MyCGAL::Hull;
    Vertex3 p0(0,0,0,0);
    Vertex3 p1(1,0,0,1);
    Vertex3 p2(0,1,0,2);
    Vertex3 p3(0,0,1,3);
    Vertex3 p4(0.2,0.2,0.2,4);
    Vertex3 p5(0.3,0.2,0.4,5);
    Vertex3 p6(1,1,1,6);
    Vertex3 p7(1,1,0,7);
    Vertex3 p8(1,0,1,8);
    Vertex3 p9(0,1,1,9);
    std::vector<Vertex3*> points;
    points.push_back(&p0);
    points.push_back(&p1);
    points.push_back(&p2);
    points.push_back(&p3);
    //points.push_back(&p4);
    //points.push_back(&p5);
    //points.push_back(&p6);
    //points.push_back(&p7);
    //points.push_back(&p8);
    //points.push_back(&p9);
    ConvexHull3 convexHull3D;
    convexHull3D.Generate(points);
    std::vector<std::array<size_t,3>> faces=convexHull3D.outputFaceIndexes();
    std::ofstream out("result\\test.obj");
    for(int k=0;k<points.size();k++){
        out<<"v "<<points[k]->X()<<" "<<points[k]->Y()<<" "<<points[k]->Z()<<std::endl;
    }
    for(int k=0;k<faces.size();k++){
        out<<"f "<<faces[k][0]+1<<" "<<faces[k][1]+1<<" "<<faces[k][2]+1<<std::endl;
    }
}
//************************************
// Method:    string_replace
// FullName:  string_replace
// Access:    public
// Returns:   void
// Qualifier: 把字符串的strsrc替换成strdst
// Parameter: std::string & strBig
// Parameter: const std::string & strsrc
// Parameter: const std::string & strdst
//————————————————
//版权声明：本文为CSDN博主「jota」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/shaoyiju/article/details/78377132
//************************************
void string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst)
{
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();

    while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
    {
        strBig.replace( pos, srclen, strdst );
        pos += dstlen;
    }
}

//************************************
// Method:    GetFileOrURLShortName
// FullName:  GetFileOrURLShortName
// Access:    public
// Returns:   std::string
// Qualifier: 获取路径或URL的文件名（包括后缀，如 C:\Test\abc.xyz --> abc.xyz）
// Parameter: std::string strFullName
//————————————————
//版权声明：本文为CSDN博主「jota」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/shaoyiju/article/details/78377132
//************************************
std::string GetPathOrURLShortName(std::string strFullName)
{
    if (strFullName.empty())
    {
        return "";
    }

    string_replace(strFullName, "/", "\\");

    std::string::size_type iPos = strFullName.find_last_of('\\') + 1;

    return strFullName.substr(iPos, strFullName.length() - iPos);
}
std::string GetPathOrURLDir(std::string strFullName)
{
    if (strFullName.empty())
    {
        return "";
    }

    string_replace(strFullName, "/", "\\");

    std::string::size_type iPos = strFullName.find_last_of('\\') + 1;

    return strFullName.substr(0,iPos);
}

void Test::testConvexHull3DInputObj(std::string objPath)
{
    tinyobj::attrib_t attribDst;
    std::vector<tinyobj::shape_t> shapesDst;
    std::vector<tinyobj::material_t> materialsDst;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attribDst, &shapesDst, &materialsDst, &warn, &err, objPath.c_str());

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }
    std::vector<size_t> originFaces;
    size_t index_offset=0;
    for(size_t i=0;i<shapesDst[0].mesh.num_face_vertices.size();i++){
        size_t faceSize=shapesDst[0].mesh.num_face_vertices[i];
        if(faceSize!=3){
            std::cerr<<"error!"<<std::endl;
        }
        size_t idx0 = shapesDst[0].mesh.indices[index_offset+0].vertex_index;
        size_t idx1 = shapesDst[0].mesh.indices[index_offset+1].vertex_index;
        size_t idx2 = shapesDst[0].mesh.indices[index_offset+2].vertex_index;
        originFaces.emplace_back(idx0);
        originFaces.emplace_back(idx1);
        originFaces.emplace_back(idx2);
        index_offset+=3;
    }
    std::string name=GetPathOrURLShortName(objPath);
    std::string originSaveName="result\\"+name;



    using namespace MyCGAL::Primitives;
    using namespace MyCGAL::Hull;
    ConvexHull3 convexHull3D(attribDst.vertices);
    std::vector<std::array<size_t,3>> faces=convexHull3D.outputFaceIndexes();
    std::string convexHullSaveName="result\\convexHull"+name;


    ObjOperator<float>::saveObjOutput(attribDst.vertices,originFaces,originSaveName);
    ObjOperator<float>::saveObjOutput(attribDst.vertices,faces,convexHullSaveName);
}
static std::string GetFileBasename(const std::string& FileName)
{
    if(FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(0, FileName.find_last_of("."));
    return "";
}
bool WriteMat(const std::string& filename, const std::vector<tinyobj::material_t>& materials) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp) {
        fprintf(stderr, "Failed to open file [ %s ] for write.\n", filename.c_str());
        return false;
    }

    for (size_t i = 0; i < materials.size(); i++) {

        tinyobj::material_t mat = materials[i];

        fprintf(fp, "newmtl %s\n", mat.name.c_str());
        fprintf(fp, "Ka %f %f %f\n", mat.ambient[0], mat.ambient[1], mat.ambient[2]);
        fprintf(fp, "Kd %f %f %f\n", mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
        fprintf(fp, "Ks %f %f %f\n", mat.specular[0], mat.specular[1], mat.specular[2]);
        fprintf(fp, "Kt %f %f %f\n", mat.transmittance[0], mat.specular[1], mat.specular[2]);
        fprintf(fp, "Ke %f %f %f\n", mat.emission[0], mat.emission[1], mat.emission[2]);
        fprintf(fp, "Ns %f\n", mat.shininess);
        fprintf(fp, "Ni %f\n", mat.ior);
        fprintf(fp, "illum %d\n", mat.illum);
        fprintf(fp, "\n");
        // @todo { texture }
    }

    fclose(fp);

    return true;
}

bool WriteObj(const std::string& filename, const tinyobj::attrib_t& attributes, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials, bool coordTransform) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp) {
        fprintf(stderr, "Failed to open file [ %s ] for write.\n", filename.c_str());
        return false;
    }

    std::string basename = GetFileBasename(filename);
    std::string material_filename = basename + ".mtl";

    int prev_material_id = -1;

    fprintf(fp, "mtllib %s\n\n", material_filename.c_str());

    // facevarying vtx
    for (size_t k = 0; k < attributes.vertices.size(); k+=3) {
        if (coordTransform) {
            fprintf(fp, "v %f %f %f\n",
                    attributes.vertices[k + 0],
                    attributes.vertices[k + 2],
                    -attributes.vertices[k + 1]);
        } else {
            fprintf(fp, "v %f %f %f\n",
                    attributes.vertices[k + 0],
                    attributes.vertices[k + 1],
                    attributes.vertices[k + 2]);
        }
    }

    fprintf(fp, "\n");

    // facevarying normal
    for (size_t k = 0; k < attributes.normals.size(); k += 3) {
        if (coordTransform) {
            fprintf(fp, "vn %f %f %f\n",
                    attributes.normals[k + 0],
                    attributes.normals[k + 2],
                    -attributes.normals[k + 1]);
        } else {
            fprintf(fp, "vn %f %f %f\n",
                    attributes.normals[k + 0],
                    attributes.normals[k + 1],
                    attributes.normals[k + 2]);
        }
    }

    fprintf(fp, "\n");

    // facevarying texcoord
    for (size_t k = 0; k < attributes.texcoords.size(); k += 2) {
        fprintf(fp, "vt %f %f\n",
                attributes.texcoords[k + 0],
                attributes.texcoords[k + 1]);
    }

    for (size_t i = 0; i < shapes.size(); i++) {
        fprintf(fp, "\n");

        if (shapes[i].name.empty()) {
            fprintf(fp, "g Unknown\n");
        } else {
            fprintf(fp, "g %s\n", shapes[i].name.c_str());
        }

        bool has_vn = false;
        bool has_vt = false;
        // Assumes normals and textures are set shape-wise.
        if(shapes[i].mesh.indices.size() > 0){
            has_vn = shapes[i].mesh.indices[0].normal_index != -1;
            has_vt = shapes[i].mesh.indices[0].texcoord_index != -1;
        }

        // face
        int face_index = 0;
        for (size_t k = 0; k < shapes[i].mesh.indices.size(); k += shapes[i].mesh.num_face_vertices[face_index++]) {
            // Check Materials
            int material_id = shapes[i].mesh.material_ids[face_index];
            if (material_id != prev_material_id) {
                std::string material_name = materials[material_id].name;
                fprintf(fp, "usemtl %s\n", material_name.c_str());
                prev_material_id = material_id;
            }

            unsigned char v_per_f = shapes[i].mesh.num_face_vertices[face_index];
            // Imperformant, but if you want to have variable vertices per face, you need some kind of a dynamic loop.
            fprintf(fp, "f");
            for(int l = 0; l < v_per_f; l++){
                const tinyobj::index_t& ref = shapes[i].mesh.indices[k + l];
                if(has_vn && has_vt){
                    // v0/t0/vn0
                    fprintf(fp, " %d/%d/%d", ref.vertex_index + 1, ref.texcoord_index + 1, ref.normal_index + 1);
                    continue;
                }
                if(has_vn && !has_vt){
                    // v0//vn0
                    fprintf(fp, " %d//%d", ref.vertex_index + 1, ref.normal_index + 1);
                    continue;
                }
                if(!has_vn && has_vt){
                    // v0/vt0
                    fprintf(fp, " %d/%d", ref.vertex_index + 1, ref.texcoord_index + 1);
                    continue;
                }
                if(!has_vn && !has_vt){
                    // v0 v1 v2
                    fprintf(fp, " %d", ref.vertex_index + 1);
                    continue;
                }
            }
            fprintf(fp, "\n");
        }
    }

    fclose(fp);

    //
    // Write material file
    //
    bool ret = WriteMat(material_filename, materials);

    return ret;
}
void RegionsToMeshes(MyCGAL::Voronoi::VoronoiMesh3<MyCGAL::Primitives::Vertex3>*voronoi,
                     Shape &shape,
                     Material &material,
                     Attribute &attribute)
{

    using namespace MyCGAL::Primitives;
    using namespace MyCGAL::Hull;
    using namespace MyCGAL::Voronoi;
    int index_offset = 0;
    for(VoronoiRegion<Vertex3>* region : voronoi->Regions())
    {
        bool draw = true;
        if(voronoi->getInputed()){
            BBox3<double> &bbox = voronoi->bbox;

            for(DelaunayCell<Vertex3>* cell : region->Cells())
            {
                if (!BBox3<double>::Inside(Vector3<double>(cell->CircumCenter()[0],cell->CircumCenter()[1],cell->CircumCenter()[2]),bbox))
                {
                    draw = false;
                    break;
                }
            }
        }
        if (!draw) continue;
        std::shared_ptr<ConvexHull3> hull = std::make_shared<ConvexHull3>();
        for(DelaunayCell<Vertex3>* cell : region->Cells())
        {
            hull->AddVert(cell->CircumCenter().X(), cell->CircumCenter().Y(), cell->CircumCenter().Z());
        }

        //If you find the convex hull of the voronoi region it
        //can be used to make a triangle mesh.

        
        hull->compute(false);
        tinyobj::shape_t st;
        st.name=std::to_string(shape.size());
        
        for (int i = 0; i < hull->Simplexs().size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {

                float x = hull->Simplexs()[i]->Vertices()[j]->X();
                float y = hull->Simplexs()[i]->Vertices()[j]->Y();
                float z = hull->Simplexs()[i]->Vertices()[j]->Z();

                attribute.vertices.push_back(x);
                attribute.vertices.push_back(y);
                attribute.vertices.push_back(z);
            }
            attribute.normals.push_back(hull->Simplexs()[i]->Normal()[0]);
            attribute.normals.push_back(hull->Simplexs()[i]->Normal()[1]);
            attribute.normals.push_back(hull->Simplexs()[i]->Normal()[2]);

            if (hull->Simplexs()[i]->IsNormalFlipped())
            {
                tinyobj::index_t index0;
                tinyobj::index_t index1;
                tinyobj::index_t index2;
                index0.texcoord_index=-1;
                index0.normal_index=i+index_offset/3;
                index0.vertex_index=(i * 3 + 2+index_offset);
                index1.texcoord_index=-1;
                index1.normal_index=i+index_offset/3;
                index1.vertex_index=(i * 3 + 1+index_offset);
                index2.texcoord_index=-1;
                index2.normal_index=i+index_offset/3;
                index2.vertex_index=(i * 3 + 0+index_offset);
                st.mesh.indices.push_back(index0);
                st.mesh.indices.push_back(index1);
                st.mesh.indices.push_back(index2);
            }
            else
            {
                tinyobj::index_t index0;
                tinyobj::index_t index1;
                tinyobj::index_t index2;
                index0.texcoord_index=-1;
                index0.normal_index=i+index_offset/3;
                index0.vertex_index=(i * 3 + 2+index_offset);
                index1.texcoord_index=-1;
                index1.normal_index=i+index_offset/3;
                index1.vertex_index=(i * 3 + 1+index_offset);
                index2.texcoord_index=-1;
                index2.normal_index=i+index_offset/3;
                index2.vertex_index=(i * 3 + 0+index_offset);
                st.mesh.indices.push_back(index0);
                st.mesh.indices.push_back(index1);
                st.mesh.indices.push_back(index2);
            }
            st.mesh.num_face_vertices.push_back(3);
            st.mesh.material_ids.push_back(-1);
            st.mesh.smoothing_group_ids.push_back(-1);
        }
        index_offset+=st.mesh.indices.size();
        shape.push_back(st);
    }

}
void Test::testVoronoi3DInputObj(std::string objPath)
{
    tinyobj::attrib_t attribDst;
    std::vector<tinyobj::shape_t> shapesDst;
    std::vector<tinyobj::material_t> materialsDst;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attribDst, &shapesDst, &materialsDst, &warn, &err, objPath.c_str());

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }
    std::vector<size_t> originFaces;
    size_t index_offset=0;
    for(size_t i=0;i<shapesDst[0].mesh.num_face_vertices.size();i++){
        size_t faceSize=shapesDst[0].mesh.num_face_vertices[i];
        if(faceSize!=3){
            std::cerr<<"error!"<<std::endl;
        }
        size_t idx0 = shapesDst[0].mesh.indices[index_offset+0].vertex_index;
        size_t idx1 = shapesDst[0].mesh.indices[index_offset+1].vertex_index;
        size_t idx2 = shapesDst[0].mesh.indices[index_offset+2].vertex_index;
        originFaces.emplace_back(idx0);
        originFaces.emplace_back(idx1);
        originFaces.emplace_back(idx2);
        index_offset+=3;
    }
    std::string name=GetPathOrURLShortName(objPath);
    QDir dir = QFileInfo(QString::fromStdString(objPath)).absoluteDir();
    dir.cdUp();
    QString absolute = dir.absolutePath();
    std::string originSaveName=absolute.toStdString()+"/result/"+name;
    std::string saveName=absolute.toStdString()+"/result/voronoi"+name;

    using namespace MyCGAL::Primitives;
    using namespace MyCGAL::Hull;
    using namespace MyCGAL::Voronoi;
    std::shared_ptr<VoronoiMesh3<Vertex3>> voronoi(new VoronoiMesh3<Vertex3>(attribDst.vertices));
    voronoi->compute();
    Shape shape;
    Material material;
    Attribute attribute;
    RegionsToMeshes(voronoi.get(),shape,material,attribute);
    ObjOperator<float>::saveObjOutput(attribDst.vertices,originFaces,originSaveName);
    WriteObj(saveName,attribute,shape,material,false);
}

void Test::testSphereConvexHull3D()
{
    int hNum=20;
    int vNum=20;
    double r=1.0f;
    std::vector<double> vertices;
    for(int i=0;i<=vNum;i++){
        if(i==0){
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(r);
            continue;
        }else if(i==vNum){
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(-r);
            continue;
        }
        for(int j=0;j<hNum;j++){
            double x=r*sin(M_PI*(double)i/vNum)*cos(2*M_PI*(double)j/hNum);
            double y=r*sin(M_PI*(double)i/vNum)*sin(2*M_PI*(double)j/hNum);
            double z=r*cos(M_PI*(double)i/vNum);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    using namespace MyCGAL::Primitives;
    using namespace MyCGAL::Hull;
    ConvexHull3 convexHull3D(vertices);
    //convexHull3D.setCreateAnimation(true);
    std::vector<std::array<size_t,3>> faces=convexHull3D.outputFaceIndexes();
    std::queue<std::vector<Simplex<Vertex3>*>>& animation=convexHull3D.animation;
    int count=0;
    while(!animation.empty()){
        std::vector<Simplex<Vertex3>*> simplexes= animation.front();
        animation.pop();
        std::vector<std::array<size_t,3>> result;
        for(Simplex<Vertex3>* f : simplexes)
        {
            std::array<size_t,3> face={f->Vertices()[0]->Id(),f->Vertices()[1]->Id(),f->Vertices()[2]->Id()};
            if(f->IsNormalFlipped()){
                face={f->Vertices()[1]->Id(),f->Vertices()[0]->Id(),f->Vertices()[2]->Id()};
            }
            result.emplace_back(face);
            delete f->Vertices()[0];
            delete f->Vertices()[1];
            delete f->Vertices()[2];
            delete f;
        }
        ObjOperator<double>::saveObjOutput(vertices, result,"result\\sphere"+std::to_string(count)+".obj");
        count++;
    }
    ObjOperator<double>::saveObjOutput(vertices,std::vector<size_t>(),"result\\Sphere.obj");
    ObjOperator<double>::saveObjOutput(vertices,faces,"result\\convexHullSphere.obj");
}

void Test::testPlaneConvexHull3D()
{
    std::array<double,3> p0={2,0,0};
    std::array<double,3> p1={1,5,0};
    std::array<double,3> p2={0,1,0};
    std::array<double,3> p3={2,3,0};
    std::array<double,3> p4={0,9,0};
    std::array<double,3> p5={6,0,0};
    std::array<double,3> p6={-7,0,0};
    std::array<double,3> p7={1,6,0};
    std::vector<std::array<double,3>> vertices;
    vertices.push_back(p0);
    vertices.push_back(p1);
    vertices.push_back(p2);
    vertices.push_back(p3);
    vertices.push_back(p4);
    vertices.push_back(p5);
    vertices.push_back(p6);
    vertices.push_back(p7);
    using namespace MyCGAL::Primitives;
    using namespace MyCGAL::Hull;
    ConvexHull3 convexHull3D(vertices);
    std::vector<std::array<size_t,3>> faces=convexHull3D.outputFaceIndexes();
    ObjOperator<double>::saveObjOutput(vertices,std::vector<size_t>(),"result\\Plane.obj");
    ObjOperator<double>::saveObjOutput(vertices,faces,"result\\convexHullPlane.obj");
}

void Test::testDuplicateConvexHull3D()
{
    std::array<double,3> p0={0,0,0};
    std::array<double,3> p1={1,0,0};
    std::array<double,3> p2={0,1,0};
    std::array<double,3> p3={0,0,1};
    std::array<double,3> p4={0,0,1};
    std::array<double,3> p5={1,0,0};
    std::array<double,3> p6={0,1,0};
    std::array<double,3> p7={1,1,0};
    std::vector<std::array<double,3>> vertices;
    vertices.push_back(p0);
    vertices.push_back(p1);
    vertices.push_back(p2);
    vertices.push_back(p3);
    vertices.push_back(p4);
    vertices.push_back(p5);
    vertices.push_back(p6);
    vertices.push_back(p7);
    using namespace MyCGAL::Primitives;
    using namespace MyCGAL::Hull;
    ConvexHull3 convexHull3D(vertices);
    std::vector<std::array<size_t,3>> faces=convexHull3D.outputFaceIndexes();
    ObjOperator<double>::saveObjOutput(vertices,std::vector<size_t>(),"result\\Duplicate.obj");
    ObjOperator<double>::saveObjOutput(vertices,faces,"result\\convexHullDuplicate.obj");

}

void Test::testRemove()
{
    std::vector<int> v;
    v.push_back(2);
    v.push_back(3);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    std::vector<int>::iterator ret = std::remove(v.begin(), v.end(), 2) ;
    ret = std::remove(v.begin(), ret, 3) ;
    for(int k=0;k<v.size();k++){
        std::cout<<v[k]<<std::endl;
    }
    std::cout<<"----"<<std::endl;
    v.erase(ret,v.end());
    std::vector<int>(v).swap(v);
    for(int k=0;k<v.size();k++){
        std::cout<<v[k]<<std::endl;
    }
}
std::list<int>::iterator findIt(int t,std::list<int>& ll){
    for(std::list<int>::iterator it=ll.begin();it!=ll.end();++it){
        if(*it==t)return it;
    }
    return ll.end();
}
void delRecursive(std::list<int>::iterator& tb,std::vector<int>& toDel,std::list<int>& ll){
    int t=*tb;
    if(*tb%2==0){
        toDel.push_back(t);
    }
    std::list<int>::iterator it=findIt(t*2,ll);
    if(it!=ll.end()){
        delRecursive(it,toDel,ll);
    }
}
void Test::testList()
{
    std::list<int> ll;
    ll.push_back(1);
    ll.push_back(8);
    ll.push_back(11);
    ll.push_back(22);
    ll.push_back(33);
    ll.push_back(44);
    ll.push_back(55);
    ll.push_back(4);
    ll.push_back(77);
    ll.push_back(88);
    
    //    for(std::list<int>::iterator tb=ll.begin();tb!=ll.end();++tb){
    //			std::vector<int> toDel;
    //            delRecursive(tb,toDel,ll);
    //            for(int k=0;k<toDel.size();k++){
    //                std::list<int>::iterator it=findIt(toDel[k],ll);
    //                if(it!=ll.end())ll.erase(it);
    //            }
    //    }
    std::list<int>::iterator te=ll.end();
    ll.erase(--ll.end());
    std::cout<<(te==ll.end())<<std::endl;
}

void Test::testUnique()
{
    std::vector<std::array<size_t,3>> A={{3,1,2},{1,6,2},{5,2,3},{2,3,1},{2,3,5},{4,3,6}};
    std::for_each(A.begin(),A.end(),[&](std::array<size_t,3>& array){
        std::sort(array.begin(),array.end());
    });
    //    std::for_each(t.begin(),t.end(),[&](std::array<size_t,3>& array){
    //       std::cout<<array[0]<<","<<array[1]<<","<<array[2]<<std::endl;
    //    });
    //    std::cout<<"========"<<std::endl;
//    std::sort(A.begin(),A.end(),[&](const std::array<size_t,3>& a,const std::array<size_t,3>& b){
//        if(a[0]<b[0]){
//            return true;
//        }else if(a[0]==b[0]){
//            if(a[1]<b[1]){
//                return true;
//            }else if(a[1]==b[1]){
//                return a[2]<b[2];
//            }else{
//                return false;
//            }
//        }else{
//            return false;
//        }
//    });
    std::for_each(A.begin(),A.end(),[&](std::array<size_t,3>& array){
        std::cout<<array[0]<<","<<array[1]<<","<<array[2]<<std::endl;
    });
    std::cout<<"========"<<std::endl;
    std::vector<std::array<size_t,3>> C=A;
    std::sort(C.begin(),C.end(),[&](const std::array<size_t,3>& a,const std::array<size_t,3>& b){
        if(a[0]<b[0]){
            return true;
        }else if(a[0]==b[0]){
            if(a[1]<b[1]){
                return true;
            }else if(a[1]==b[1]){
                return a[2]<b[2];
            }else{
                return false;
            }
        }else{
            return false;
        }
    });
    C.erase(std::unique(C.begin(), C.end()), C.end());
    std::for_each(C.begin(),C.end(),[&](std::array<size_t,3>& array){
        std::cout<<array[0]<<","<<array[1]<<","<<array[2]<<std::endl;
    });
    std::cout<<"========"<<std::endl;
    std::vector<size_t> ia;
    ia.reserve(C.size());
    std::vector<size_t> ic;
    ic.reserve(A.size());
    std::transform(C.begin(), C.end(),
                   std::back_inserter(ia),
                   [&](std::array<size_t,3>& array) { return std::distance(A.begin(),
                                                     std::find(A.begin(), A.end(), array));
    });//C=A(ia);
    std::cout<<"C=A(ia)"<<std::endl;
    std::for_each(ia.begin(),ia.end(),[&](size_t& i){
        std::cout<<i<<" ";
    });
    std::cout<<std::endl;
    std::cout<<"--------"<<std::endl;
    std::transform(A.begin(), A.end(),
                   std::back_inserter(ic),
                   [&](std::array<size_t,3>& array) { return std::distance(C.begin(),
                                                     std::find(C.begin(), C.end(), array));
    });//A=C(ic);
    std::cout<<"A=C(ic)"<<std::endl;
    std::for_each(ic.begin(),ic.end(),[&](size_t& i){
        std::cout<<i<<" ";
    });
    std::cout<<std::endl;
    std::cout<<"--------"<<std::endl;
}
