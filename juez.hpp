#pragma once
#include "tablita.hpp"
#include "estado.hpp"
//aqui van las reglas del juego, declaraciones para que los cpp las comprendan
//le puse que hace cada una para que se entienda

//revisa si la ficha se puede mover un paso a ese destino
bool validarPaso(const Estado &estado, const GRafo &grafo, int ficha, int destino);

//revisa si el puma puede saltar a ese destino, si se puede deja en casillaCapturada
//la casilla del perro q se come
bool validarSalto(const Estado &estado, const GRafoCapturas &mapeoCapturas, int ficha, int destino, int &casillaCapturada);

//mueve la ficha un paso 
void aplicarPaso(Estado &estado, int ficha, int destino);

//el puma salta, el perro de casillaCapturada se muere y sube el contador
void aplicarSalto(Estado &estado, int ficha, int destino, int casillaCapturada);

//true si el puma ya se comio 6 o mas, la win condition
bool pumaGano(const Estado &estado);

//true si el puma no tiene ni pasos ni saltos win condition de los canes
bool pumaAcorralado(const Estado &estado, const GRafo &grafo, const GRafoCapturas &mapeoCapturas);