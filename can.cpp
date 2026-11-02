#include <unistd.h>
#include <iostream>
#include "can.hpp"
#include "livritoReglas.hpp"
#include "azar.hpp"
#include "consola.hpp"
#include "synchro.hpp"
using namespace std;

constexpr int intentosPorCiclo = 3;

// intenta un paso al azar
static bool pruebaDcaminar(Mesa &mesa, int idCan, int &miCasilla, mt19937 &dadito, bool &muerto){
    //esto saca los pasos de la casilla del tablerode una mesa espef y trae a un vector
    const vector<int> &pasos = mesa.tablero->vecinos[miCasilla];
    //este otro hace el elegir random tipo, se llama al dadito q da un gigantisimo numeri
    //y se divide en 4 con resto para saber cual usamos
    int destino = pasos[dadito() % pasos.size()];
    int caminata[2] = {miCasilla, destino};
    //lo de arriva igual q el puma es caminar nomas

    if(!cerrarCasillas(mesa, caminata, 2)){
        return false;
    }

    // si el puma se lo comio
    if(mesa.estado.casilla[miCasilla] != idCan){
        muerto = true;
        abrirCasillas(mesa, caminata, 2);
        return false;
    }

    bool valido = validarPaso(mesa.estado, *mesa.tablero, idCan, destino);
    if(valido){
        aplicarPaso(mesa.estado, idCan, destino);
    }
    abrirCasillas(mesa, caminata, 2);

    if(valido){
        {
            Guard guardiaConsola(&mutexConsola);
            cout << "mesa " << mesa.idMesa << ": el can " << idCan << " paso de " << miCasilla << " a " << destino << endl;
        }
        miCasilla = destino;
        avisarMovimiento(mesa);
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

        //esto para un error q me salio de k cuando despierta tira igual aunq ya se durmio....
        if(partidaTerminada(mesa)){
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