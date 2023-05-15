#ifndef POINT_H
#define POINT_H

#include<string>

using namespace std;

template<class T, int ndim>
class Point {
private:
    T coords[ndim]{0};
public:
    Point(){};
    Point(T coords[ndim]){
        for (int i = 0; i < ndim; i++){
            this->coords[i] = coords[i];
        }
    }
    Point(const Point<T, ndim> &other){
        this->coords = other.coords;
    }
    T& operator[](int index){
        return coords[index];
    }
    Point<T, ndim> getCoords(Point<T, ndim> &other, T function(T, T)){
        T tmp[ndim];
        for (int i = 0; i < ndim; i++){
            tmp[i] = function(this->coords[i], other[i]);
        }
        return Point<T, ndim>(tmp);
    }
    Point<T, ndim> operator+(Point<T, ndim> &other){
        T tmp[ndim];
        for (int i = 0; i < ndim; i++){
            tmp[i] = this->coords[i] + other[i];
        }
        return Point<T, ndim>(tmp);
    }
    T norm(){
        T sum = 0;
        for (int i = 0; i < ndim; i++){
            sum += coords[i] * coords[i];
        }
        return sqrt(sum);
    }
    T distance(Point<T, ndim> &other){
        T sum = 0;
        for (int i = 0; i < ndim; i++){
            sum += (coords[i] - other[i]) * (coords[i] - other[i]);
        }
        return sqrt(sum);
    }
    friend ostream& operator<<(ostream &os, Point<T, ndim> &point){
        os << "(";
        for (int i = 0; i < ndim; i++){
            os << point[i];
            if (i < ndim - 1) os << ", ";
        }
        os << ")";
        return os;
    }
};

#endif