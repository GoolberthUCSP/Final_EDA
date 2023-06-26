#include<iostream>
#include<chrono>
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
  auto start = std::chrono::high_resolution_clock::now();
  BallTree<float, DIM> balltree(50, "cleaned.csv");
  //run(balltree);
  auto end = std::chrono::high_resolution_clock::now();
  cout << "Tiempo de ejecución: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << endl;
}