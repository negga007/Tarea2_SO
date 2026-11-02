#include <iostream>
#include "juez.hpp"
#include "livritoReglas.hpp"
#include "consola.hpp"
#include "synchro.hpp"
using namespace std;

constexpr int topeMovimientos = 500; //para q no sea infinito, 500 es empate

void *hiloJuez(void *argumento){
    Mesa &mesa = *(Mesa *)argumento;

    while(!partidaTerminada(mesa)){
        int movimientos;
        {
            Guard guardia(&mesa.mutexAviso);
            movimientos = mesa.movimientos;
        }

        //tablero
        Estado estadoTablero;
        int resultado = res_Nadie;
        for(auto &candado : mesa.mutexCasilla){
            pthread_mutex_lock(&candado);
        }
        estadoTablero = mesa.estado;

        // quien gano
        if(pumaGano(estadoTablero)){
            resultado = res_Puma;
        }else if(pumaAcorralado(estadoTablero, *mesa.tablero, *mesa.mapeoCapturas)){
            resultado = res_Perros;
        }else if(movimientos >= topeMovimientos){
            resultado = res_Empate;
        }

        //macamos el finish con el tablero sellado
        if(resultado != res_Nadie){
            Guard guardia(&mesa.mutexAviso);
            mesa.terminada = true;
            mesa.resultado = resultado;
        }
        for(auto &candado : mesa.mutexCasilla){
            pthread_mutex_unlock(&candado);
        }

        if(resultado != res_Nadie){
            Guard guardiaConsola(&mutexConsola);
            if(resultado == res_Puma){
                cout << "mesa " << mesa.idMesa << ": gano el puma con " ;
                cout << estadoTablero.contadorCapturas << " capturas" << endl;
            }else if(resultado == res_Perros){
                cout << "mesa " << mesa.idMesa;
                cout << ": ganaron los canes, el puma quedo sin movimientos" << endl;
            }else{
                cout << "mesa " << mesa.idMesa << ": empate, " ;
                cout << topeMovimientos << " movimientos" << endl;
            }
            break;
        }

        // espera un movimiento
        Guard guardia(&mesa.mutexAviso);
        while(mesa.movimientos == movimientos && !mesa.terminada){
            pthread_cond_wait(&mesa.avisoAlJuez, &mesa.mutexAviso);
        }
    }
    return NULL;
}