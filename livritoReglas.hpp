#pragma once
#include "tablita.hpp"
#include "estado.hpp"

//revisa si la ficha se puede mover un paso a ese destino
bool validarPaso(const Estado &estado, const GRafo &grafo, int ficha, int destino);

//la casilla del perro q se come
bool validarSalto(const Estado &estado, const GRafoCapturas &mapeoCapturas, int ficha, int destino, int &casillaCapturada);

//mueve la fichay un paso 
void aplicarPaso(Estado &estado, int ficha, int destino);

//el puma salta, el perro de casillaCapturada se muere y sube el contador
void aplicarSalto(Estado &estado, int ficha, int destino, int casillaCapturada);

//true si el puma ya se comio 6 o mas, la win condition
bool pumaGano(const Estado &estado);

//true si el puma no tiene ni pasos ni saltos win condition de los canes
bool pumaAcorralado(const Estado &estado, const GRafo &grafo, const GRafoCapturas &mapeoCapturas);