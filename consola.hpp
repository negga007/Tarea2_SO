#pragma once
#include <pthread.h>
//un solo mutex pa la consola, asi las lineas de los 13 hilos no se mezclan en la impresion
inline pthread_mutex_t mutexConsola = PTHREAD_MUTEX_INITIALIZER;