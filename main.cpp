#include<iostream>
#include<fstream>
#include <iostream>
#include "source/database.h"
#include "source/balltree.h"
#include "source/eigen/Eigen/Dense"

#define DIM 10

using namespace std;

void run(BallTree<float, DIM> &balltree){
}

int main()
{
  BallTree<float, DIM> balltree(100);
  db.load("dataset.csv");
  db.indexing();
  run(db);
}