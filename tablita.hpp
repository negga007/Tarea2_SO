#pragma once //para q no se repita el coso
#include <vector>

constexpr int TamannoTabla = 5; //es 5 ya que el coso es 5x5
constexpr int TotalPosiciones = 28; //el 5x5 mas los 3 de trono
constexpr int Trono_up = 27;
constexpr int Trono_izq = 25;
constexpr int Trono_derecha = 26;//posiciones contando 0
constexpr int pos_Invalida = -1;


constexpr int AsignadorDID(int x, int y){
    return (y * TamannoTabla) + x; 
    /*algo ais
    y
    |6 7 8
    |3 4 5          ejemplo con tamaño = 3
    |0 1 2          y luego con los tronos arriva
    -----------------x
    
    */
}
struct GRafo{
    std::vector<int> vecinos[TotalPosiciones];
};
struct Capturas{
    int posCapturada;
    int aterrizaje;
};

struct GRafoCapturas{
    std::vector<Capturas> Captura[TotalPosiciones];//matriz pa ver si puede capturar desde esa pos
};


void construirCapturas(GRafoCapturas &grafoCapturas);
void contrsuirGrafo(GRafo &grafo);