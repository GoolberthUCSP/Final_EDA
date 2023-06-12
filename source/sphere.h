#ifndef SPHERE_H
#define SPHERE_H

#include "record.h"

template<class T, int ndim>
struct Sphere{
    Point<T, ndim> center;
    T radius;
    Sphere(Point<T, ndim> &center, T radius){
        this->center = Point<T, ndim>(center);
        this->radius = radius;
    }
    Sphere() : center(Point<T, ndim>()), radius(0) {}
    void operator=(Sphere<T, ndim> other){
        center = other.center;
        radius = other.radius;
    }
    friend ostream& operator<<(ostream &os, Sphere<T, ndim> &sphere){
        os << "Center: " << sphere.center << "\nRadius: " << sphere.radius << endl;
        return os;
    }
};

#endif // SPHERE_H