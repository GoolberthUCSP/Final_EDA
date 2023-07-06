#include<iostream>
#include "source/balltree.h"

#define DIM 13

using namespace std;

int main()
{
  //Solo los primeros 18100 registros tienen nombre
  BallTree<DIM> balltree(200, "songs_final.csv");
  cout << "Indexing time: " << balltree.getIndexingTime() << endl;
  
  vector<string> knn = balltree.knnQuery(1, 20);
  cout << "Knn time: " << balltree.getKnnTime() << endl;
  for (auto i : knn) cout << i << endl;
  cout << endl;

}