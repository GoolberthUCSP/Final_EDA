#ifndef SPHERE_H
#define SPHERE_H

#include <eigen3/Eigen/Dense>

using namespace Eigen;

template<int ndim>
struct Sphere{

    using Sphere_ = Sphere<ndim>;

    VectorXf center;
    float radius;
    Sphere(VectorXf center, float radius) : center(center), radius(radius) {}
    Sphere() : center(VectorXf(ndim)), radius(0) {}
    void operator=(Sphere_ sphere){
        center = sphere.center;
        radius = sphere.radius;
    }
    friend ostream& operator<<(ostream &os, Sphere_ &sphere){
        os << sphere.center.transpose() << "\t" << sphere.radius;
        return os;
    }
    /*
        @brief Calcula la distancia entre la esfera y un punto
        @param point: punto al que se le calculará la distancia
        @return distancia entre la esfera y el punto
    */
    float distance(VectorXf &point){
        return (center-point).norm()-radius;
    }
    /*
        @brief Calcula la distancia entre dos esferas
        @param sphere: esfera a la que se le calculará la distancia
        @return distancia entre las dos esferas
    */
    float distance(Sphere_ &sphere){
        return (center-sphere.center).norm()-radius-sphere.radius;
    }
};

#endif // SPHERE_H