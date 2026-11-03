#pragma once
#include <pthread.h>
#include <vector>
#include "mesa.hpp"
#include "synchro.hpp"

//cosas q comparten el torneo y el hilo del ctrl+c
inline pthread_mutex_t mutexTorneo = PTHREAD_MUTEX_INITIALIZER;
inline bool apagando = false;             //true cuando llego el ctrl+c
inline std::vector<Mesa *> mesasActivas;  //las q estan jugando, las cuida mutexTorneo

//mete la mesa a las activas, false si ya nos estan apagando
inline bool meterMesa(Mesa *mesa){
    Guard guardia(&mutexTorneo);
    if(apagando){
        return false;
    }
    mesasActivas.push_back(mesa);
    return true;
}

//saca la mesa de las activas antes de destruirla
inline void sacarMesa(Mesa *mesa){
    Guard guardia(&mutexTorneo);
    for(size_t i = 0; i < mesasActivas.size(); i++){
        if(mesasActivas[i] == mesa){
            mesasActivas.erase(mesasActivas.begin() + i);
            break;
        }
    }
}