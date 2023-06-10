#include<iostream>
#include "source/balltree.h"

#define DIM 11

using namespace std;

void run(BallTree<float, DIM> &balltree){
  cout << "BallTree created" << endl;
}

int main()
{
  BallTree<float, DIM> balltree(500);
  run(balltree);
}