#ifndef SPHERE_H
#define SPHERE_H

#include <eigen/Dense>

using namespace Eigen;

template<int ndim>
struct Sphere{

    using Sphere_ = Sphere<ndim>;

    VectorXf center;
    float radius;
    Sphere(VectorXf center, float radius) : center(center), radius(radius) {}
    Sphere() : center(VectorXf(ndim)), radius(0) {}
    void operator=(Sphere_ &sphere){
        center = sphere.center;
        radius = sphere.radius;
    }
    friend ostream& operator<<(ostream &os, Sphere_ &sphere){
        os << sphere.center.transpose() << "\t" << sphere.radius;
        return os;
    }
    float distance(VectorXf &point){
        return (center-point).norm()-radius;
    }
    float distance(Sphere_ &sphere){
        return (center-sphere.center).norm()-radius-sphere.radius;
    }
};

#endif // SPHERE_H