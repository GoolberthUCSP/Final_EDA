#include<iostream>
#include "source/balltree.h"

#define DIM 13

using namespace std;

int main()
{
  //OJO: Solo los primeros 18100 registros tienen nombre
  int k = 200;
  int id= 4318;
  string name = "Cyberpurple"; 

  BallTree<DIM> balltree(600, "songs_final.csv");
  cout << "Tiempo de indexación: " << balltree.getIndexingTime() << " ns" << endl;
  cout << "------------WITH-ID------------\n";
  for (int i=0; i<1; i++){
    vector<string> res= balltree.knnQuery(id, k);
    cout << "Tiempo de consulta: " << balltree.getKnnTime() << " ns" << endl;
    for (int j=0; j<res.size(); j+=res.size()/5){
      cout << res[j] << endl;
    }
  }
  cout << "-----------WITH-NAME-----------\n";
  for (int i=0; i<1; i++){
    vector<string> res= balltree.knnQuery(name, k);
    cout << "Tiempo de consulta: " << balltree.getKnnTime() << " ns" << endl;
    for (int j=0; j<res.size(); j+=res.size()/5){
      cout << res[j] << endl;
    }
  }
  cout << "-------------LINEAR------------\n";
  vector<string> res= balltree.linearKnnQuery(id, k);
  cout << "Tiempo de consulta: " << balltree.getLinearKnnTime() << " ns" << endl;
  for (int j=0; j<res.size(); j+=res.size()/5){
    cout << res[j] << endl;
  }
}