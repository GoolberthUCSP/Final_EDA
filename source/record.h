#ifndef RECORD_H
#define RECORD_H

#define EIGEN_USE_THREADS
#include <eigen3/Eigen/Dense>
#include <string>

using namespace Eigen;
using namespace std;

/*
    @brief Estructura que representa una canción
    @param ndim: Dimensión de las características de la canción
*/
template <int ndim>
struct Record{
    
    VectorXf point;
    int id;
    string name;
    float proyFactor;

    using Record_ = Record<ndim>;

    Record() : point(ndim), id(-1), name("") {}
    Record(int id, VectorXf point, string name) : point(point), id(id), name(name) {}
    Record(const Record_ &other) : point(other.point), id(other.id), name(other.name) {}

    Record_& operator=(const Record_ &other){
        point = other.point;
        id = other.id;
        name = other.name;
        return *this;
    }
    int getDimension() const { return ndim; }
    /*
        @brief Calcula la distancia entre el record y otro record
        @param other: record al que se le calculará la distancia
        @return distancia entre el record y el otro record
    */
    float distance(const Record_ &other) const {
        return (point - other.point).norm();
    }
    /*
        @brief Calcula la distancia entre el record y un punto
        @param other: punto al que se le calculará la distancia
        @return distancia entre el record y el punto
    */
    float distance(VectorXf &other) const {
        return (point - other).norm();
    }
    friend ostream &operator<<(ostream &os, Record_ &record){
        os << record.id << record.name << "\t" << record.point.transpose();
        return os;
    }
};

#endif // RECORD_H