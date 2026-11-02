#include <unistd.h>
#include <iostream>
#include "puma.hpp"
#include "livritoReglas.hpp"
#include "azar.hpp"
#include "consola.hpp"
#include "synchro.hpp"
using namespace std;

constexpr int intentosPorCiclo = 3; //cuantas jugadas prueba antes de volver a dormir

//escoge una jugada al azar y trata de hacerla, true si lo logro
static bool jugarPumita(Mesa &mesa, mt19937 &dadito){
    int origen = mesa.estado.posicion[id_delpuma]; //solo el puma la cambia, asi q la puede leer
    const auto &pasos = mesa.tablero->vecinos[origen];
    const auto &saltos = mesa.mapeoCapturas->Captura[origen];

    int opciones = pasos.size() + saltos.size();
    int eleccion = dadito() % opciones;

    //primero van los pasos y despues los saltos
    if(eleccion < (int)pasos.size()){
        int destino = pasos[eleccion];
        int caminata[2] = {origen, destino};

        if(!cerrarCasillas(mesa, caminata, 2)){
            return false; //alguna estaba ocupada
        }
        bool valido = validarPaso(mesa.estado, *mesa.tablero, id_delpuma, destino);
        if(valido){
            aplicarPaso(mesa.estado, id_delpuma, destino);
        }
        abrirCasillas(mesa, caminata, 2);

        if(valido){
            {
                Guard guardiaConsola(&mutexConsola);
                cout << "mesa " << mesa.idMesa << ": el puma paso de " << origen << " a " << destino << endl;
            }
            avisarMovimiento(mesa);
        }
        return valido;
    }

    //si no era paso es salto
    const auto &salto = saltos[eleccion - pasos.size()];
    int destino = salto.aterrizaje;
    int caminata[3] = {origen, salto.posCapturada, destino};

    if(!cerrarCasillas(mesa, caminata, 3)){
        return false;
    }
    int casillaCapturada = 0;
    int perroComido = Vacio;
    bool valido = validarSalto(mesa.estado, *mesa.mapeoCapturas, id_delpuma, destino, casillaCapturada);
    if(valido){
        perroComido = mesa.estado.casilla[casillaCapturada]; //hay q verlo antes de aplicar
        aplicarSalto(mesa.estado, id_delpuma, destino, casillaCapturada);
    }
    abrirCasillas(mesa, caminata, 3);

    if(valido){
        {
            Guard guardiaConsola(&mutexConsola);
            cout << "mesa " << mesa.idMesa << ": el puma salto de " << origen << " a " << destino
                 << " y se comio al perro " << perroComido << endl;
        }
        avisarMovimiento(mesa);
    }
    return valido;
}

void *hiloPuma(void *argumento){
    Mesa &mesa = *(Mesa *)argumento;
    mt19937 dadito = crearDadito(*mesa.config, mesa.idMesa * 100 + id_delpuma);

    while(!partidaTerminada(mesa)){
        usleep(tirarRetardo(dadito, *mesa.config) * 1000); //de ms a microsegundos
        
        if(partidaTerminada(mesa)) break;

        for(int intento = 0; intento < intentosPorCiclo; intento++){
            if(jugarPumita(mesa, dadito)){
                break; //ya se movio, a dormir
            }
        }
    }
    return NULL;
}