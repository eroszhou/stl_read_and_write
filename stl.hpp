#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
template <typename T>
class ThreeVector {
public:
  enum { X=0, Y=1, Z=2, NUM_COORDINATES=3, SIZE=NUM_COORDINATES };

  ThreeVector() : data{0, 0, 0} {}
  explicit ThreeVector(T x) : data{x, 0, 0} {}
  ThreeVector(T x, T y) : data{x, y, 0} {}
  ThreeVector(T x, T y, T z) : data{x, y, z} {}
  inline ThreeVector(const ThreeVector & p) : data{p.x(), p.y(), p.z()} {}
  inline ThreeVector(ThreeVector &&) = default;
  inline ThreeVector &operator = (const ThreeVector & p) {
    set(p.x(), p.y(), p.z());
    return *this;
  }
  inline ~ThreeVector(){}

  inline T operator () (int i) const { return data[i];}
  inline T operator [] (int i) const { return data[i]; }
  inline T & operator () (int i) { return data[i];}
  inline T & operator [] (int i) { return data[i]; }
  inline T x() const { return (*this)[X]; }
  inline T y() const { return (*this)[Y]; }
  inline T z() const { return (*this)[Z]; }
  inline void setX(T x) { (*this)[X] = x; }
  inline void setY(T y) { (*this)[Y] = y; }
  inline void setZ(T z) { (*this)[Z] = z; }
  inline void set(T x, T y, T z) { (*this)[X] = x; (*this)[Y] = y; (*this)[Z] = z;}

  inline T getX() const { return (*this)[X]; }
  inline T getY() const { return (*this)[Y]; }
  inline T getZ() const { return (*this)[Z]; }

protected:
  T data[3];
};

template <typename T>
class Triangle{
    public:
    Triangle(){}
    Triangle(std::ifstream& inf){
        float data[12];
        inf.read((char*)data, sizeof(float)*12);
        short int e;
        inf.read((char*)&e, sizeof(short int));

        this->mNormalVector = ThreeVector<T>(T(data[0]), T(data[1]), T(data[2]));
        this->mVertex[0] = ThreeVector<T>(T(data[3]), T(data[4]), T(data[5]));
        this->mVertex[1] = ThreeVector<T>(T(data[6]), T(data[7]), T(data[8]));
        this->mVertex[2] = ThreeVector<T>(T(data[9]), T(data[10]), T(data[11]));
    }
    ~Triangle(){}
    void write2ascii(std::ofstream& outf) const {
        outf << "facet normal " << this->mNormalVector[0] << " " << this->mNormalVector[1] << " " << this->mNormalVector[2] << "\n";
        outf << "    outer loop\n";
        outf << "        vertex " << this->mVertex[0][0] << " " << this->mVertex[0][1] << " " << this->mVertex[0][2] << "\n";
        outf << "        vertex " << this->mVertex[1][0] << " " << this->mVertex[1][1] << " " << this->mVertex[1][2] << "\n";
        outf << "        vertex " << this->mVertex[2][0] << " " << this->mVertex[2][1] << " " << this->mVertex[2][2] << "\n";
        outf << "    endloop\n";
        outf << "endfacet\n";
    }
    void write2binary(std::ofstream& outf) const {
        float data[12];
        for(int i = 0; i < 3; i++){
            data[i] = this->mNormalVector[i];
            data[3 + i] = this->mVertex[0][i];
            data[2*3 + i] = this->mVertex[1][i];
            data[3*3 + i] = this->mVertex[2][i];
        }
        outf.write((char*)data, sizeof(float)*12);
        short int e = 0;
        outf.write((char*)&e, sizeof(short int));
    }
    public:
    ThreeVector<T> mNormalVector;
    ThreeVector<T> mVertex[3];
};

template <typename T>
class STL{
    public:
    STL(){}
    STL(const STL& stl){ this->mSTL = stl.mSTL;}
    STL(const std::string stlFile) {this->read(stlFile);}
    void append(Triangle<T>& triangle) {this->mSTL.push_back(triangle);}
    void append(STL& stl) {this->mSTL.insert(this->mSTL.end(), stl.begin(), stl.end());}
    bool read(const std::string stlFile){
        std::vector<Triangle<T>>().swap(this->mSTL);
        std::ifstream inf(stlFile, std::ios::binary);
        if(!inf){
            std::cout << "Can not read file " << stlFile << "\n";
            return false;
        }
        inf.seekg(0, inf.end);
	    size_t length = inf.tellg();
	    inf.seekg(0, inf.beg);
        int len = (length - 84)/50;
        unsigned char header[80];
        inf.read((char*)header, sizeof(unsigned char)*80);
        unsigned int nTriangle;
        inf.read((char*)&nTriangle, sizeof(unsigned int));
        if(nTriangle != len){
            std::cout << "File size does not match the number of triangles " << len << " vs " << nTriangle << "\n";
            return false;
        }
        for(int i = 0; i < nTriangle; i++){
            this->mSTL.emplace_back(inf);
        }
        inf.close();
        return true;
    }
    void write2binary(const std::string stlFile) const {
        std::ofstream outf(stlFile, std::ios::binary);
        unsigned char header[80];
        memset(header, 0, sizeof(unsigned char)*80);
        outf.write((char*)header, sizeof(unsigned char)*80);
        unsigned int len = this->mSTL.size();
        outf.write((char*)&len, sizeof(unsigned int));
        for(auto& triangle : this->mSTL){
            triangle.write2binary(outf);
        }
        outf.close();
    }
    void write2ascii(const std::string stlFile) const {
        std::ofstream outf(stlFile);
        size_t pos = stlFile.find_last_of(".");
        std::string name = stlFile.substr(0, pos);
        outf << "solid " << name << "\n";
        for(auto& triangle : this->mSTL){
            triangle.write2ascii(outf);
        }
        outf << "endsolid " << name;
        outf.close();
    }
    ~STL(){}
    public:
    std::vector<Triangle<T>> mSTL;
};
