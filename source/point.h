#ifndef POINT_H
#define POINT_H

#include<cmath>

using namespace std;

template<class T, int ndim>
class Point {
private:
    T *coords;
public:
    using Point_ = Point<T, ndim>;

    Point(){
        coords= new T[ndim]{0};
    }
    Point(T coords[ndim]){
        this->coords = coords;
    }
    Point(Point_ &other){
        coords = new T[ndim];
        for (int i = 0; i < ndim; i++){
            coords[i] = other[i];
        }
    }
    T& operator[](int index){
        return coords[index];
    }
    Point_ operator+(Point_ &other){
        T *tmp = new T[ndim];
        for (int i = 0; i < ndim; i++){
            tmp[i] = this->coords[i] + other[i];
        }
        return Point_(tmp);
    }
    T norm(){
        T sum = 0;
        for (int i = 0; i < ndim; i++){
            sum += coords[i] * coords[i];
        }
        return sqrt(sum);
    }
    T distance(Point_ &other){
        T sum = 0;
        for (int i = 0; i < ndim; i++){
            sum += (coords[i] - other[i]) * (coords[i] - other[i]);
        }
        return sqrt(sum);
    }
    T dotProduct(Point_ &other){
        T sum = 0;
        for (int i = 0; i < ndim; i++){
            sum += coords[i] * other[i];
        }
        return sum;
    }
    Point_ operator/(int scalar){
        T *tmp = new T[ndim];
        for (int i = 0; i < ndim; i++){
            tmp[i] = coords[i] / scalar;
        }
        return Point_(tmp);
    }
    void operator=(Point_ other){
        for (int i = 0; i < ndim; i++){
            coords[i] = other[i];
        }
    }
    friend ostream& operator<<(ostream &os, Point_ &point){
        os << "(";
        for (int i = 0; i < ndim; i++){
            os << point[i];
            if (i < ndim - 1) os << ",";
        }
        os << ")";
        return os;
    }
    void setCoords(T coords[ndim]){
        this->coords = coords;
    }
};

#endif