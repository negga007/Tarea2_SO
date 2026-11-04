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

//revisa si ya se acabo la partida, se llama con mutexAviso cerrado
static void revisarFinal(Mesa &mesa){
    if(pumaGano(mesa.estado)){
        mesa.terminada = true;
        mesa.resultado = res_Puma;
    }else if(pumaAcorralado(mesa.estado, *mesa.tablero, *mesa.mapeoCapturas)){
        mesa.terminada = true;
        mesa.resultado = res_Perros;
    }else if(mesa.movimientos >= topeMovimientos){
        mesa.terminada = true;
        mesa.resultado = res_Empate;
    }
}

//el juez valida el paso y lo aplica, las casillas ya vienen cerradas
bool juezValidaPaso(Mesa &mesa, int ficha, int destino){
    Guard guardia(&mesa.mutexAviso);

    //si ya termino o se cayo alguien nadie mas se mueve
    if(mesa.terminada || mesa.abandonaron != res_Nadie){
        return false;
    }
    if(!validarPaso(mesa.estado, *mesa.tablero, ficha, destino)){
        return false;
    }
    int origen = mesa.estado.posicion[ficha];
    aplicarPaso(mesa.estado, ficha, destino);

    string quien = "el puma";
    if(ficha != id_delpuma){
        quien = "el can " + to_string(ficha);
    }
    string texto = quien + " paso de " + to_string(origen) + " a " + to_string(destino);

    //la jugada y su foto quedan anotadas en el mismo orden en q se aplicaron
    mesa.movimientos++;
    mesa.jugadas.push_back(texto + "\n" + pintarTablero(mesa.estado));
    revisarFinal(mesa);
    {
        Guard guardiaConsola(&mutexConsola);
        cout << "mesa " << mesa.idMesa << ": " << texto << endl;
    }
    pthread_cond_signal(&mesa.avisoAlJuez);
    return true;
}

//lo mismo pero con el salto del puma
bool juezValidaSalto(Mesa &mesa, int destino){
    Guard guardia(&mesa.mutexAviso);

    if(mesa.terminada || mesa.abandonaron != res_Nadie){
        return false;
    }
    int casillaCapturada = 0;
    if(!validarSalto(mesa.estado, *mesa.mapeoCapturas, id_delpuma, destino, casillaCapturada)){
        return false;
    }
    int origen = mesa.estado.posicion[id_delpuma];
    int perroComido = mesa.estado.casilla[casillaCapturada]; //hay q verlo antes de aplicar
    aplicarSalto(mesa.estado, id_delpuma, destino, casillaCapturada);

    string texto = "el puma salto de " + to_string(origen) + " a " + to_string(destino)
                 + " y se comio al perro " + to_string(perroComido);

    mesa.movimientos++;
    mesa.jugadas.push_back(texto + "\n" + pintarTablero(mesa.estado));
    revisarFinal(mesa);
    {
        Guard guardiaConsola(&mutexConsola);
        cout << "mesa " << mesa.idMesa << ": " << texto << endl;
    }
    pthread_cond_signal(&mesa.avisoAlJuez);
    return true;
}

void *hiloJuez(void *datosDlaMesa){
    Mesa &mesa = *(Mesa *)datosDlaMesa;

    iniciarMesa(mesa, mesa.idMesa, *mesa.tablero, *mesa.mapeoCapturas, *mesa.config);
    {
        Guard guardia(&mesa.mutexInit);
        mesa.iniciada = true;
        pthread_cond_broadcast(&mesa.condInit);
    }

    ofstream log("mesa_" + to_string(mesa.idMesa) + ".txt"); //el log de esta mesa
    int numeroJugada = 0;
    int resultado = res_Nadie;
    int capturas = 0;
    bool porAbandono = false;
    bool seguir = true;

    //si el puma ya parte acorralado (config) se acaba altiro
    {
        Guard guardia(&mesa.mutexAviso);
        revisarFinal(mesa);
    }

    //esta seccion es para esperar jugadas y escribirlas en el log
    while(seguir){
        vector<string> pendientes;
        {
            Guard guardia(&mesa.mutexAviso);
            while(mesa.jugadas.empty() && !mesa.terminada && mesa.abandonaron == res_Nadie){
                pthread_cond_wait(&mesa.avisoAlJuez, &mesa.mutexAviso);
            }
            pendientes.swap(mesa.jugadas);

            //si alguien se cayo y nadie habia ganado, el juez cierra la partida
            if(mesa.abandonaron != res_Nadie && !mesa.terminada){
                mesa.terminada = true;
                mesa.resultado = mesa.abandonaron;
                porAbandono = true;
            }
            seguir = !mesa.terminada;
            resultado = mesa.resultado;
            capturas = mesa.estado.contadorCapturas;
        }

        for(const auto &jugada : pendientes){
            numeroJugada++;
            log << "mov " << numeroJugada << ": " << jugada << "\n";
        }
    }

    //esta seccion es para escribir el resultado final en el log y en la consola
    string fin;
    if(resultado == res_Nadie){
        fin = "partida cortada por ctrl+c";
    }else if(porAbandono){
        if(resultado == res_Puma){
            fin = "un jugador se cayo, gana por abandono el puma";
        }else{
            fin = "un jugador se cayo, gana por abandono el equipo de los canes";
        }
    }else if(resultado == res_Puma){
        fin = "gano el puma con " + to_string(capturas) + " capturas";
    }else if(resultado == res_Perros){
        fin = "ganaron los canes, el puma quedo sin movimientos";
    }else{
        fin = "empate, " + to_string(topeMovimientos) + " movimientos";
    }

    log << "fin: " << fin << "\n";

    if(resultado != res_Nadie){
        Guard guardiaConsola(&mutexConsola);
        cout << "mesa " << mesa.idMesa << ": " << fin << endl;
    }
    return NULL;
}