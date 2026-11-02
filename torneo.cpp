#include <vector>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include "tablita.hpp"
#include "mesa.hpp"
#include "estado.hpp"
#include "config.hpp"
#include "puma.hpp"
#include "can.hpp"
#include "juez.hpp"
using namespace std;

int main(int argc, char *argv[]){
    //ommmmm recordatorio ./torneo N M [config.txt]
    if(argc < 3 || argc > 4){
        cerr << "Uso: ./torneo N M [config.txt]" << endl;
        return 1;
    }
    int numeroN = atoi(argv[1]); //cuantos jugadores llegan
    int mesasM = atoi(argv[2]);  //max de mesas a la vez

    //si no son numeros atoi da 0
    if(numeroN <= 0 || mesasM <= 0){
        cerr << "N y M tienen q ser numeros mayores a 0" << endl;
        return 1;
    }
    
    //valores por defecto, si viene config los pisa
    configuraxion config;
    if(argc == 4){
        if(!leerConfig(argv[3], config)){
            return 1; //leerConfig ya aviso cual fue el error
        }
    }

    //contriir tablero
    GRafo tablero;
    contrsuirGrafo(tablero);
    //craftear mapa de capturas
    GRafoCapturas mapeoCapturas;
    construirCapturas(mapeoCapturas);
    //estado inicial de la partida

    cout << numeroN << " jugadores, " << mesasM << " mesas" << endl;
    
    
    //prueba para una mesa
    Mesa mesa;
    iniciarMesa(mesa, 0, tablero, mapeoCapturas, config);

    pthread_t hiloDelJuez;
    pthread_t hiloDelPuma;
    pthread_t hilosCanes[TotalPerros];
    rutCan rutas[TotalPerros]; //una ruta por can

    pthread_create(&hiloDelJuez, NULL, hiloJuez, &mesa);
    pthread_create(&hiloDelPuma, NULL, hiloPuma, &mesa);
    for(int idCan = 0; idCan < TotalPerros; idCan++){
        rutas[idCan].mesa = &mesa;
        rutas[idCan].idCan = idCan;
        pthread_create(&hilosCanes[idCan], NULL, hiloCan, &rutas[idCan]);
    }

    pthread_join(hiloDelJuez, NULL);
    pthread_join(hiloDelPuma, NULL);
    for(auto &hilo : hilosCanes){
        pthread_join(hilo, NULL);
    }

    destruirMesa(mesa);
    return 0;
}