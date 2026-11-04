#include <unistd.h>
#include <iostream>
#include "puma.hpp"
#include "azar.hpp"
#include "juez.hpp"
using namespace std;

constexpr int intentosPorCiclo = 3; //cuantas jugadas prueba antes de volver a dormir

//escoge una jugada al azar y trata de hacerla, true si lo logro
static bool jugarPumita(Mesa &mesa, mt19937 &dadito){
    int origen = mesa.estado.posicion[id_delpuma]; //solo el puma la cambia, asi q la puede leer
    const auto &pasos = mesa.tablero->vecinos[origen];
    const auto &saltos = mesa.mapeoCapturas->Captura[origen];

    int opciones = pasos.size() + saltos.size();
    if(opciones == 0){
        return false; //no hay a donde moverse
    }
    int eleccion = dadito() % opciones;

    //primero van los pasos y despues los saltos
    if(eleccion < (int)pasos.size()){
        int destino = pasos[eleccion];
        int caminata[2] = {origen, destino};

        if(!cerrarCasillas(mesa, caminata, 2)){
            return false; //alguna estaba ocupada
        }
        bool valido = juezValidaPaso(mesa, id_delpuma, destino);
        abrirCasillas(mesa, caminata, 2);
        return valido;
    }

    //si no era paso es salto
    const auto &salto = saltos[eleccion - pasos.size()];
    int destino = salto.aterrizaje;
    int caminata[3] = {origen, salto.posCapturada, destino};

    if(!cerrarCasillas(mesa, caminata, 3)){
        return false;
    }
    bool valido = juezValidaSalto(mesa, destino);
    abrirCasillas(mesa, caminata, 3);
    return valido;
}

void *hiloPuma(void *argumento){
    Mesa &mesa = *(Mesa *)argumento;
    mt19937 dadito = crearDadito(*mesa.config, mesa.idMesa * 100 + id_delpuma);

    while(!partidaTerminada(mesa)){
        usleep(tirarRetardo(dadito, *mesa.config) * 1000); //de ms a microsegundos

        if(partidaTerminada(mesa)) break;
      
        if(mesa.idMesa == mesaQueSeCae && fichaQueSeCae == id_delpuma){
            avisarCaida(mesa, res_Perros); //cae el puma, es para probar caida esto
            break;
        }

        for(int intento = 0; intento < intentosPorCiclo; intento++){
            if(jugarPumita(mesa, dadito)){
                break; //ya se movio, a dormir
            }
        }
    }
    return NULL;
}