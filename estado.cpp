#include "estado.hpp"
#include "config.hpp"

//iniciamos to vacio
void iniciarEstado(Estado &estado, const configuraxion &config){
    for(int casilla = 0; casilla < TotalPosiciones; casilla++){
        estado.casilla[casilla] = Vacio;
    }
    estado.contadorCapturas = 0; //win condition

    for(int perro = 0; perro < TotalPerros; perro++){
        estado.posicion[perro] = config.posPerros[perro];   //el perro esta en esa casilla
        estado.casilla[config.posPerros[perro]] = perro;    //esa casilla tiene a ese perro
    }

    //ahora el puma, que es el 12, en la casilla que dice el config
    estado.posicion[id_delpuma] = config.posPuma;
    estado.casilla[config.posPuma] = id_delpuma;
}