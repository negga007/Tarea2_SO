#pragma once
#include <string>
#include "tablita.hpp"
#include "estado.hpp"
using namespace std;
//valores del defecto, el config dsp los pisa con lo que traiga
struct configuraxion {
    int semilla = 42;                  //semilla del azar
    int retardoMin = 50;               //retardo minimo
    int retardoMax = 300;              //retardo maximo
    int posPuma = Trono_up;            //choza donde parte el Puma
    int posPerros[TotalPerros] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 14}; //casilla iniciales
};

//lee el config.txt y llena el Config, devuelve false si algo viene malo
bool leerConfig(const string &ruta, configuraxion &config);