#pragma once
#include <pthread.h>
#include "tablita.hpp"
#include "estado.hpp"
#include "config.hpp"
#include <string>
#include <vector>
#include "synchro.hpp"

//como termino la partida, resultado es res para la chaviza
constexpr int res_Nadie = 0;   
constexpr int res_Puma = 1;
constexpr int res_Perros = 2;
constexpr int res_Empate = 3;
//esta seccion es solo para probar el abandono
constexpr int mesaQueSeCae = -1;
constexpr int fichaQueSeCae = -1; 

//una mesa es una partida entera con todo lo q comparten sus fichas
struct Mesa {
    int idMesa;                                    //numero de la mesa
    Estado estado;                                 //el tablero compartido
    pthread_mutex_t mutexCasilla[TotalPosiciones]; //un mutex por casilla
    pthread_mutex_t mutexAviso;                    //cuida el tablero, las jugadas, movimientos, terminada y resultado.
    pthread_cond_t avisoAlJuez;                    //las fichas avisan aca q se movieron
    int movimientos;                               //n mov
    bool terminada;                                
    int resultado;
    int abandonaron = res_Nadie;
    pthread_mutex_t mutexInit;
    pthread_cond_t condInit;
    bool iniciada = false;
    vector<string> jugadas;                                   

    //esto solo se lee, nadie lo cambia asi q no necesita mutex
    const GRafo *tablero;
    const GRafoCapturas *mapeoCapturas;
    const configuraxion *config;
};

//deja la mesa lista
void iniciarMesa(Mesa &mesa, int idMesa, const GRafo &tablero, const GRafoCapturas &mapeoCapturas, const configuraxion &config);

//cuando la partida termina se destruye to 
void destruirMesa(Mesa &mesa);

//cierra los mutex de esas casilla con trylock si alguna estaba ocupada abre las q alcanzo a cerrar
bool cerrarCasillas(Mesa &mesa, const int casillas[], int cuantas);

//abre los mutex de esas casillas
void abrirCasillas(Mesa &mesa, const int casillas[], int cuantas);


bool partidaTerminada(Mesa &mesa);
//esta seccion es para avisarle al juez que una ficha se cayo
inline void avisarCaida(Mesa &mesa, int ganador){
    Guard guardia(&mesa.mutexAviso);
    mesa.abandonaron = ganador;
    pthread_cond_signal(&mesa.avisoAlJuez);
}