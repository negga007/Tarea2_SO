#pragma once
#include <random>
#include "config.hpp"
using namespace std;

//cada ficha con su dadito, asi no comparten semilla entre hilos
inline mt19937 crearDadito(const configuraxion &config, int idFicha){
    return mt19937(config.semilla + idFicha);
}

//tira el dadito y devuelve cuanto espera en ms, entre retardoMin y retardoMax
inline int tirarRetardo(mt19937 &dadito, const configuraxion &config){
    uniform_int_distribution<int> rangito(config.retardoMin, config.retardoMax); 
    return rangito(dadito);
}