#include <vector>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include "tablita.hpp"
using namespace std;

void Caminos(GRafo &grafo, int pos1, int pos2){
    if(pos1 < 0 || pos1 >= TotalPosiciones || pos2 < 0 || pos2 >= TotalPosiciones || pos1 == pos2){
        cout << "posicion invalida" << endl;
        return;
    }else{

        if(find(grafo.vecinos[pos1].begin(), grafo.vecinos[pos1].end(), pos2) != grafo.vecinos[pos1].end()){
            return;
        }else{
            if(find(grafo.vecinos[pos1].begin(), grafo.vecinos[pos1].end(), pos2) == grafo.vecinos[pos1].end()){
            grafo.vecinos[pos1].push_back(pos2);
            }
            
            if(find(grafo.vecinos[pos2].begin(), grafo.vecinos[pos2].end(), pos1) == grafo.vecinos[pos2].end()){
            grafo.vecinos[pos2].push_back(pos1);
            }
        }
    }

}

int main(){
    cout << "hello github" << endl;
    GRafo grafo;    
    Caminos(grafo,0,5);
    cout << "vecinos de 0: " << grafo.vecinos[0][0] << " vecinos de 5: " << grafo.vecinos[5][0] << endl;
    Caminos(grafo,0,5);
    cout << "cantidad de vecinos de 0: " << grafo.vecinos[0].size() << endl;
    Caminos(grafo, 0, 99);
    Caminos(grafo, 3, 3);
    cout << "vecinos de 0 tras llamadas invalidas: " << grafo.vecinos[0].size() << " (esperado 1)" << endl;
    cout << "vecinos de 3 tras llamadas invalidas: " << grafo.vecinos[3].size() << " (esperado 0)" << endl;
    
    return 0;
}