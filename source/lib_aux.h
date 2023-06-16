#include<algorithm>
#include "record.h"

template<class T, int ndim>
T getProyFactor(Point<T, ndim> &eigenVect, Point<T, ndim> &point){
    // Retorna el factor de proyección de point sobre eigenVect
    // Factor de proyección: punto.eigenVect / eigenVect.norm2()
    return eigenVect.dotProduct(point) / eigenVect.norm2();
}

template<class T, int ndim>
void sortByProyFactor(vector<Record<T,ndim>*> &records, Point<T, ndim> &eigenVect){
    // Ordena records por el factor de proyección sobre eigenVect
    sort(records.begin(), records.end(), [&eigenVect](Record<T, ndim> *a, Record<T, ndim> *b){
        return getProyFactor(eigenVect, a->getPoint()) < getProyFactor(eigenVect, b->getPoint());
    });
}