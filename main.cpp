#include<iostream>
#include "source/balltree.h"

#define DIM 13

using namespace std;

void run(BallTree<DIM> &balltree){
  vector<string> songs= balltree.by_atribute("energy", 0.75);
  for (auto song : songs){
    cout << song << endl;
  }
}

int main()
{
  BallTree<DIM> balltree(200, "songs_final.csv");
  cout << "Indexing time: " << balltree.getIndexingTime() << endl;
  //run(balltree);
}