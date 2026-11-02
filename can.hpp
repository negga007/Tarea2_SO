#pragma once
#include "mesa.hpp"

// lo q recibe cada hilo can
struct rutCan {
    Mesa *mesa;
    int idCan;
};

// hilo de un can
void *hiloCan(void *argumento);