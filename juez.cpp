#include <iostream>
#include "juez.hpp"
#include <fstream>
#include <string>
#include "dibujadorDlogs.hpp"
#include "livritoReglas.hpp"
#include "consola.hpp"
#include "synchro.hpp"
using namespace std;

constexpr int topeMovimientos = 500; //para q no sea infinito, 500 es empate

void *hiloJuez(void *datosDlaMesa){
    Mesa &mesa = *(Mesa *)datosDlaMesa;
    
    ofstream log("mesa_" + to_string(mesa.idMesa) + ".txt"); //el log de esta mesa
    int numeroJugada = 0;

    while(!partidaTerminada(mesa)){
        int movimientos;
        int abandonaron;
        {
            Guard guardia(&mesa.mutexAviso);
            movimientos = mesa.movimientos;
            abandonaron = mesa.abandonaron;
        }

        //tablero
        Estado estadoTablero;
        int resultado = res_Nadie;
        for(auto &candado : mesa.mutexCasilla){
            pthread_mutex_lock(&candado);
        }
        estadoTablero = mesa.estado;

        vector<string> pendientes;
        {
            Guard guardiaJugadas(&mesa.mutexAviso);
            pendientes.swap(mesa.jugadas);
        }

        // quien gano
        if(abandonaron != res_Nadie){
            resultado = abandonaron;
        }else if(pumaGano(estadoTablero)){
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

        //escribimos el log 
        for(const auto &jugada : pendientes){
            numeroJugada++;
            log << "mov " << numeroJugada << ": " << jugada << "\n";
        }
        if(!pendientes.empty()){
            log << pintarTablero(estadoTablero) << "\n";
        }

        if(resultado != res_Nadie){
            
            Guard guardiaConsola(&mutexConsola);

            if(abandonaron != res_Nadie){
                cout << "mesa " << mesa.idMesa << ": un jugador se cayo, gana por abandono el ";
                if(resultado == res_Puma){
                    cout << "puma" << endl;
                }else{
                    cout << "equipo de los canes" << endl;
                }
            }else if(resultado == res_Puma){
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
        while(mesa.movimientos == movimientos && !mesa.terminada && mesa.abandonaron == res_Nadie){
            pthread_cond_wait(&mesa.avisoAlJuez, &mesa.mutexAviso);
        }
    }

        //las ultimas q llegaron justo al final
    {
        Guard guardiaJugadas(&mesa.mutexAviso);
        for(const auto &j : mesa.jugadas){
            numeroJugada++;
            log << "mov " << numeroJugada << ": " << j << "\n";
        }
        mesa.jugadas.clear();
    }
    
    return NULL;
}