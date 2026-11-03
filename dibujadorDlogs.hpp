#pragma once
#include <string>
#include "estado.hpp"
using namespace std;

//una casilla son 3 chars, " x " vacia, " P " puma, "D3 " perro
inline string pintarCasilla(const Estado &estado, int casilla){
    int quien = estado.casilla[casilla];
    if(quien == Vacio){
        return " x ";
    }
    if(quien == id_delpuma){
        return " P ";
    }
    if(quien < 10){
        return "D" + to_string(quien) + " ";
    }
    return "D" + to_string(quien);
}

//el tablero entero en texto, trono arriba y la grilla debajo
inline string pintarTablero(const Estado &estado){
    string pantalla;

    pantalla += "      " + pintarCasilla(estado, Trono_up) + "      \n";
    pantalla += "   " + pintarCasilla(estado, Trono_izq) + "   " + pintarCasilla(estado, Trono_derecha) + "   \n";

    for(int y = TamannoTabla - 1; y >= 0; y--){
        for(int x = 0; x < TamannoTabla; x++){
            pantalla += pintarCasilla(estado, AsignadorDID(x, y));
        }
        pantalla += "\n";
    }
    return pantalla;
}