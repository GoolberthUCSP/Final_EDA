#include<iostream>
#include "source/balltree.h"

#define DIM 11

using namespace std;

void run(BallTree<float, DIM> &balltree){
  vector<string> songs= balltree.by_atribute("energy", 0.75);
  for (auto song : songs){
    cout << song << endl;
  }
}

int main()
{
  BallTree<float, DIM> balltree(50, "dtest.csv");
  run(balltree);
}