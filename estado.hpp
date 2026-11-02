#pragma once
#include "tablita.hpp"
//todo esto para tener registre de qien ta en donde
constexpr int id_delpuma = 12;
constexpr int Vacio = -1;
constexpr int TotalPerros = 12;
constexpr int TotalFichas = 13;
constexpr int pos_Capturado = -99;

struct Estado {
    int casilla[TotalPosiciones];
    int posicion[TotalFichas];
    int contadorCapturas;
};

struct configuraxion; 
void iniciarEstado(Estado &estado, const configuraxion &config);