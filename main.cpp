#include<iostream>
#include "source/balltree.h"

#define DIM 13

using namespace std;

int main()
{
  //Solo los primeros 18100 registros tienen nombre
  BallTree<DIM> balltree(200, "songs_final.csv");
  cout << "Indexing time: " << balltree.getIndexingTime() << endl;

}