#include "livritoReglas.hpp"

constexpr int capturasParaGanar = 6; 

bool validarPaso(const Estado &estado, const GRafo &grafo, int ficha, int destino){
    //primero q los numeros existan, sino explota
    if(ficha < 0 || ficha >= TotalFichas){
        return false;
    }
    if(destino < 0 || destino >= TotalPosiciones){
        return false;
    }
    //si ya se lo comieron no se mueve mas
    if(estado.posicion[ficha] == pos_Capturado){
        return false;
    }
    //si hay alguien en el destino tampoco
    if(estado.casilla[destino] != Vacio){
        return false;
    }
    int origen = estado.posicion[ficha];

    //el destino tiene q estar en la lista de vecinos
    for(auto vecino : grafo.vecinos[origen]){
        if(vecino == destino){
          return true;
        }
    }
    
    return false;
}
//salto del puma
bool validarSalto(const Estado &estado, const GRafoCapturas &mapeoCapturas, int ficha, int destino, int &casillaCapturada){
    //solo el puma salta, los caninos no capturan
    if(ficha != id_delpuma){
        return false;
    }
    if(destino < 0 || destino >= TotalPosiciones){
        return false;
    }
    if(estado.casilla[destino] != Vacio){
        return false;
    }

    int origen = estado.posicion[id_delpuma];

    for(const auto &salto : mapeoCapturas.Captura[origen]){
        if(salto.aterrizaje == destino){

            int quienEsta = estado.casilla[salto.posCapturada];

            //en el medio tiene q haber un perro, ni vacio ni el mismo puma <- imposible por porsiacaso we
            if(quienEsta != Vacio && quienEsta != id_delpuma){
                casillaCapturada = salto.posCapturada;
                return true;
            }
        }
    }
    return false;
}

//aplicaciones
void aplicarPaso(Estado &estado, int ficha, int destino){
    int origen = estado.posicion[ficha];
    estado.casilla[origen] = Vacio;
    estado.casilla[destino] = ficha;
    estado.posicion[ficha] = destino;
}

void aplicarSalto(Estado &estado, int ficha, int destino, int casillaCapturada){
    int perroPobre = estado.casilla[casillaCapturada];
    estado.casilla[casillaCapturada] = Vacio;
    estado.posicion[perroPobre] = pos_Capturado;
    estado.contadorCapturas++;
    aplicarPaso(estado, ficha, destino);
}

bool pumaGano(const Estado &estado){
    return estado.contadorCapturas >= capturasParaGanar;
}

//acorralado 
bool pumaAcorralado(const Estado &estado, const GRafo &grafo, const GRafoCapturas &mapeoCapturas){

    int origen = estado.posicion[id_delpuma];

    for(auto vecino : grafo.vecinos[origen]){
        if(estado.casilla[vecino] == Vacio){
            return false; 
        }
    }

    for(const auto &salto : mapeoCapturas.Captura[origen]){
        int temp = 0; //no importa es solo para ver que no se safa matando 

        if(validarSalto(estado, mapeoCapturas, id_delpuma, salto.aterrizaje, temp)){
            return false; 
        }
    }
    return true;
}