#ifndef BALLTREE_H
#define BALLTREE_H

#include "record.h"

template<class T, int ndim>
struct Circle{
    Point<T, ndim> center;
    T radius;
    Circle(Point<T, ndim> center, T radius);
    Circle(Point<T, ndim> center, Point<T, ndim> point);
};


template<class T, int ndim>
class BallTree{
private:
    Circle<T, ndim> *circle;
    BallTree<T, ndim> *left, *right;
    vector<Record<T, ndim>*> records;
    bool isLeaf;
public:
    using Record_ = Record<T, ndim>;
    using Point_ = Point<T, ndim>;
    static constexpr int ndim_ = ndim;
    using T_ = T;

    BallTree(vector<Record<T, ndim>> &records);
    static BallTree<T, ndim>* build(vector<Record<T, ndim>> &records);
    bool insert(Record<T, ndim> &record);
    bool remove(Record<T, ndim> &record);
    bool update(Record<T, ndim> &record);
    bool search(Record<T, ndim> &record);
    bool rangeSearch(Point<T, ndim> &point, T radius);
    bool searchKNN(Point<T, ndim> &point, int k);
};

template<class T, int ndim>
Circle<T,ndim>::Circle(Point<T, ndim> center, Point<T, ndim> point){
    this->center = center;
    this->radius = center.distance(point);
}
template<class T, int ndim>
Circle<T,ndim>::Circle(Point<T, ndim> center, T radius){
    this->center = center;
    this->radius = radius;
}

#endif