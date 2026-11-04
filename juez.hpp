#pragma once
#include "mesa.hpp"

// hilo del juez
void *hiloJuez(void *argumento);

// el juez valida y aplica un paso, true si era valido (se llama con las casillas cerradas)
bool juezValidaPaso(Mesa &mesa, int ficha, int destino);

// el juez valida y aplica el salto del puma, true si era valido
bool juezValidaSalto(Mesa &mesa, int destino);