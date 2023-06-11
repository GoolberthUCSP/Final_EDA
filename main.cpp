#include<iostream>
#include "source/balltree.h"

#define DIM 11

using namespace std;

void run(BallTree<float, DIM> &balltree){
  std::cout << "BallTree created" << endl;
}

int main()
{
  BallTree<float, DIM> balltree(10, "dtest.csv");
  run(balltree);
}