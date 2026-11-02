#pragma once
#include <random>
#include "config.hpp"
using namespace std;

//cada ficha tiene su dadito propio
//asi con la misma semilla ningun hilo comparte dado con otro
inline mt19937 crearDadito(const configuraxion &config, int idFicha){
    return mt19937(config.semilla + idFicha);
}

//tira el dadito y devuelve cuanto espera en ms, entre retardoMin y retardoMax (los dos incluidos)
inline int tirarRetardo(mt19937 &dadito, const configuraxion &config){
    uniform_int_distribution<int> rangito(config.retardoMin, config.retardoMax); //el rango q queremos
    return rangito(dadito); //el dadito escoge un numero dentro del rango
}