#pragma once
#include <pthread.h>
#include <vector>
#include "mesa.hpp"
#include <signal.h>
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


//hilo duerme esperando ctrl+c, main bloquea sigint antes de crear hilos
inline void *hiloSigint(void *){
    sigset_t atrapa;
    sigemptyset(&atrapa);
    sigaddset(&atrapa, SIGINT);

    int numSennnal = 0;
    sigwait(&atrapa, &numSennnal); //aca duerme hasta q llegue el ctrl+c

    Guard guardia(&mutexTorneo);
    apagando = true; //meterMesa ya no deja entrar mas mesas
    //esta seccion es para avisarle a cada mesa activa que termine
    for(auto mesa : mesasActivas){
        Guard guardiaMesa(&mesa->mutexAviso);
        mesa->terminada = true;
        pthread_cond_broadcast(&mesa->avisoAlJuez); //pa q el juez salga del wait
    }
    return NULL;
}