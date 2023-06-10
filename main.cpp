#include<iostream>
#include<fstream>
#include <iostream>
#include "source/balltree.h"

#define DIM 10

using namespace std;

void run(BallTree<float, DIM> &balltree){
}

int main()
{
  BallTree<float, DIM> balltree(100);
  balltree.load("dataset.csv");
  balltree.build();
  run(balltree);
}