#include<iostream>
#include<cstdlib> //system
#include "source/balltree.h"

#define DIM 13

using namespace std;

void run(BallTree<DIM> &balltree){
  int k, id, opt, tmp;
  string name;
  do{
    cout << "Ingrese la opción de búsqueda:\n";
    cout << "1. Por ID\n";
    cout << "2. Por nombre\n";
    cin >> opt;
    tmp= system("clear || cls");
    if (opt == 1){
      cout << "Ingrese el ID de la canción: ";
      cin >> id;
      cout << "Ingrese el número de vecinos: ";
      cin >> k;
      vector<string> res= balltree.knnQuery(id, k);
      cout << "Tiempo de consulta: " << balltree.getKnnTime() << " ns" << endl;
      cout << "--------------------------------\n";
      for (int j=0; j<res.size(); j+=res.size()/5){
        cout << j << '\t' << res[j] << endl;
      }
    }
    else if (opt == 2){
      cout << "Ingrese el nombre de la canción: ";
      cin >> name;
      cout << "Ingrese el número de vecinos: ";
      cin >> k;
      vector<string> res= balltree.knnQuery(name, k);
      cout << "Tiempo de consulta: " << balltree.getKnnTime() << " ns" << endl;
      cout << "--------------------------------\n";
      for (int j=0; j<res.size(); j+=res.size()/5){
        cout << j << '\t' << res[j] << endl;
      }
    }
    else{
      cout << "Opción inválida\n";
    }
    cout << "¿Desea realizar otra consulta? (s/n)\n";
    cin >> name;
    tmp= system("clear || cls");
  } while (name == "s");
}

int main()
{
  //OJO: Solo los primeros 18100 registros tienen nombre
  BallTree<DIM> balltree(600, "songs_final.csv");
  cout << "Tiempo de construcción: " << balltree.getIndexingTime() << " ns" << endl;
  run(balltree);
}