#include <unistd.h>
#include <iostream>
#include "can.hpp"
#include "azar.hpp"
#include "juez.hpp"
using namespace std;

constexpr int intentosPorCiclo = 3;

// intenta un paso al azar
static bool pruebaDcaminar(Mesa &mesa, int idCan, int &miCasilla, mt19937 &dadito, bool &muerto){
    //esto saca los pasos de la casilla del tablerode una mesa espef y trae a un vector
    const vector<int> &pasos = mesa.tablero->vecinos[miCasilla];

    //esta seccion es para elegir un vecino al azar
    int destino = pasos[dadito() % pasos.size()];
    int caminata[2] = {miCasilla, destino};

    if(!cerrarCasillas(mesa, caminata, 2)){
        return false;
    }

    // si el puma se lo comio
    if(mesa.estado.casilla[miCasilla] != idCan){
        muerto = true;
        abrirCasillas(mesa, caminata, 2);
        return false;
    }

    bool valido = juezValidaPaso(mesa, idCan, destino);
    abrirCasillas(mesa, caminata, 2);

    if(valido){
        miCasilla = destino;
    }
    return valido;
}

void *hiloCan(void *argumento){
    rutCan &arg = *(rutCan *)argumento;
    Mesa &mesa = *arg.mesa;
    int idCan = arg.idCan;

    int miCasilla = mesa.config->posPerros[idCan]; 
    bool muerto = false;
    mt19937 dadito = crearDadito(*mesa.config, mesa.idMesa * 100 + idCan);

    while(!partidaTerminada(mesa) && !muerto){
        usleep(tirarRetardo(dadito, *mesa.config) * 1000);
        //esta seccion es para no jugar si la partida termino mientras dormia
        if(partidaTerminada(mesa)){
            break;
        }

        if(mesa.idMesa == mesaQueSeCae && fichaQueSeCae == idCan){
            avisarCaida(mesa, res_Puma); //cae el can, gana el puma
            break;
        }

        for(int intento = 0; intento < intentosPorCiclo && !muerto; intento++){
            if(pruebaDcaminar(mesa, idCan, miCasilla, dadito, muerto)){
                break;
            }
        }
    }
    return NULL;
}