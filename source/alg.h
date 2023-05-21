#ifndef ALG_H
#define ALG_H

#include<cmath>
#include<vector>
#include<Eigen/Dense>
#include "point.h"

//Recibe un vector de puntos y devuelve el radio y el centroide
//de la esfera que los contiene en un vector de tamaño ndim+1: vec={r, point}
template<class T, int ndim>
std::vector<Point<T,ndim>> welzl(std::vector<Point<T,ndim>> &points){

}

#endif