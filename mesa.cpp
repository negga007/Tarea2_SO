#include <algorithm>
#include "mesa.hpp"
#include "synchro.hpp"
using namespace std;


void iniciarMesa(Mesa &mesa, int idMesa, const GRafo &tablero, const GRafoCapturas &mapeoCapturas, const configuraxion &config){
    mesa.idMesa = idMesa;
    mesa.tablero = &tablero;
    mesa.mapeoCapturas = &mapeoCapturas;
    mesa.config = &config;
    iniciarEstado(mesa.estado, config);

    for(auto &candado : mesa.mutexCasilla){
        pthread_mutex_init(&candado, NULL);
    }
    pthread_mutex_init(&mesa.mutexAviso, NULL);
    pthread_cond_init(&mesa.avisoAlJuez, NULL);

    mesa.movimientos = 0;
    mesa.terminada = false;
    mesa.resultado = res_Nadie;
}

void destruirMesa(Mesa &mesa){
    for(auto &candado : mesa.mutexCasilla){
        pthread_mutex_destroy(&candado);
    }
    pthread_mutex_destroy(&mesa.mutexAviso);
    pthread_cond_destroy(&mesa.avisoAlJuez);
}
//captura de casillas para moverte
bool cerrarCasillas(Mesa &mesa, const int casillas[], int cuantas){
    int orden[3];
    for(int i = 0; i < cuantas; i++){
        orden[i] = casillas[i];
    }
    sort(orden, orden + cuantas);

    for(int i = 0; i < cuantas; i++){
        if(pthread_mutex_trylock(&mesa.mutexCasilla[orden[i]]) != 0){
            //estaba ocupada, soltamos las q ya teniamos y nos vamos
            for(int j = 0; j < i; j++){
                pthread_mutex_unlock(&mesa.mutexCasilla[orden[j]]);
            }
            return false;
        }
    }

    //si el juez ya grito ganador todos kietos
    if(partidaTerminada(mesa)){
        for(int i = 0; i < cuantas; i++){
            pthread_mutex_unlock(&mesa.mutexCasilla[orden[i]]);
        }
        return false;
    }
    return true;
}

void abrirCasillas(Mesa &mesa, const int casillas[], int cuantas){
    for(int i = 0; i < cuantas; i++){
        pthread_mutex_unlock(&mesa.mutexCasilla[casillas[i]]);
    }
}

bool partidaTerminada(Mesa &mesa){
    Guard guardia(&mesa.mutexAviso);
    return mesa.terminada;
}

void avisarMovimiento(Mesa &mesa){
    Guard guardia(&mesa.mutexAviso);
    mesa.movimientos++;
    pthread_cond_signal(&mesa.avisoAlJuez);
}