#include<algorithm>
#include "record.h"

template<int ndim>
float getProyFactor(VectorXf &eigenvect, VectorXf &point){
    return eigenvect.dot(point) / eigenvect.squaredNorm();
}

template<int ndim>
void sortByProyFactor(vector<Record<ndim>*> &records, VectorXf &eigenvect){
    // Ordena records por el factor de proyección sobre eigenvect
    sort(records.begin(), records.end(), [&eigenvect](Record<ndim> *a, Record<ndim> *b){
        return getProyFactor(eigenvect, a->getPoint()) < getProyFactor(eigenvect, b->getPoint());
    });
}