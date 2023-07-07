#include<iostream>
#include "source/balltree.h"

#define DIM 13

using namespace std;

int main()
{
  int id= 1000;
  int k = 300;
  //Solo los primeros 18100 registros tienen nombre
  BallTree<DIM> balltree(450, "songs_final.csv");
  cout << "Tiempo de indexación: " << balltree.getIndexingTime() << " ns" << endl;
  balltree.knnQuery(id, k);
  cout << "Tiempo de búsqueda: " << balltree.getKnnTime() << " ns" << endl;
  balltree.knnQuery(id, k);
  cout << "Tiempo de búsqueda: " << balltree.getKnnTime() << " ns" << endl;
  balltree.knnQuery(id, k);
  cout << "Tiempo de búsqueda: " << balltree.getKnnTime() << " ns" << endl;
  balltree.knnQuery(id, k);
  cout << "Tiempo de búsqueda: " << balltree.getKnnTime() << " ns" << endl;
  balltree.knnQuery(id, k);
  cout << "Tiempo de búsqueda: " << balltree.getKnnTime() << " ns" << endl;
}