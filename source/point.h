#ifndef POINT_H
#define POINT_H

#include<string>
#include<cmath>

using namespace std;

template<class T, int ndim>
class Point {
private:
    T coords[ndim]{0};
public:
    using Point_ = Point<T, ndim>;

    Point(){};
    Point(T coords[ndim]){
        for (int i = 0; i < ndim; i++){
            this->coords[i] = coords[i];
        }
    }
    Point(Point_ &other){
        this->coords = other.coords;
    }
    T& operator[](int index){
        return coords[index];
    }
    Point_ operator+(Point_ &other){
        T tmp[ndim];
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
    T product(Point_ &other){
        T sum = 0;
        for (int i = 0; i < ndim; i++){
            sum += coords[i] * other[i];
        }
        return sum;
    }
    friend ostream& operator<<(ostream &os, Point_ &point){
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