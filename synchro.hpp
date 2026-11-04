#pragma once
#include <pthread.h>
#include <semaphore.h>
#include <cerrno>

//guard: cierra el mutex al nacer y lo abre solo al morir
//es como un guarda pro para que si se te olvida abrir lo abre el al final de su turno xd
struct Guard {
    pthread_mutex_t *candado;

    Guard(pthread_mutex_t *mutex){
        candado = mutex;
        pthread_mutex_lock(candado);
    }
    ~Guard(){
        pthread_mutex_unlock(candado);
    }
};  

//envoltorio del semaforo
inline void semIniciar(sem_t *semaforo, unsigned int valor){
    sem_init(semaforo, 0, valor);
}

inline void semEsperar(sem_t *semaforo){
    // ctrl+c 
    while(sem_wait(semaforo) == -1 && errno == EINTR){
    }
}

inline void semAvisar(sem_t *semaforo){
    sem_post(semaforo);
}

inline void semDestruir(sem_t *semaforo){
    sem_destroy(semaforo);
}