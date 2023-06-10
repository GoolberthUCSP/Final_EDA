#include<algorithm>
#include "record.h"

template<class T, int ndim>
T getProyFactor(Point<T, ndim> &vect, Point<T, ndim> &point){
    // Retorna el factor de proyección de point sobre vect
    return vect.dotProduct(point) / vect.norm();
}

template<class T, int ndim>
void sortByProyFactor(vector<Record<T,ndim>*> &records, Point<T, ndim> &vect){
    // Ordena records por el factor de proyección sobre vect
    sort(records.begin(), records.end(), [&vect](Record<T, ndim> *a, Record<T, ndim> *b){
        return getProyFactor(vect, a->getPoint()) < getProyFactor(vect, b->getPoint());
    });
}