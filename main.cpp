#include<iostream>
#include<fstream>
#include <iostream>
#include "source/database.h"
#include "source/balltree.h"
#include "source/eigen/Eigen/Dense"

#define DIM 10

using namespace std;

void run(Database<BallTree<float, DIM>> &db);

int main()
{
  Database<BallTree<float, DIM>> db;
  db.load("dataset.csv");
  db.indexing();
  run(db);
}