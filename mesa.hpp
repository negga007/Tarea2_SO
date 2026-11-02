#pragma once
#include <pthread.h>
#include "tablita.hpp"
#include "estado.hpp"
#include "config.hpp"

//como termino la partida, resultado es res para la chaviza
constexpr int res_Nadie = 0;   
constexpr int res_Puma = 1;
constexpr int res_Perros = 2;
constexpr int res_Empate = 3;

//una mesa es una partida entera con todo lo q comparten sus fichas
struct Mesa {
    int idMesa;                                    //numero de la mesa
    Estado estado;                                 //el tablero compartido
    pthread_mutex_t mutexCasilla[TotalPosiciones]; //un mutex por casilla
    pthread_mutex_t mutexAviso;                    //cuida movimientos, terminada y resultado
    pthread_cond_t avisoAlJuez;                    //las fichas avisan aca q se movieron
    int movimientos;                               //n mov
    bool terminada;                                
    int resultado;                                

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

//avisa al Juez q movimiento valido
void avisarMovimiento(Mesa &mesa);