#pragma once //para q no se repita el coso
#include <vector>

constexpr int TamannoTabla = 5; //es 5 ya que el coso es 5x5
constexpr int TotalPosiciones = 28; //el 5x5 mas los 3 de trono
constexpr int Trono_up = 27;
constexpr int Trono_izq = 25;
constexpr int Trono_derecha = 26;//posiciones contando 0
constexpr int pos_Invalida = -1;



constexpr int AsignadorDID(int x, int y){
    return (x * TamannoTabla) + y; 
    /*algo ais
    y
    |2 5 8
    |1 4 7           ejemplo con tamaño = 3
    |0 3 6           y luego con los tronos arriva
    -----------------x
    
    */
}
struct GRafo{
    std::vector<int> vecinos[TotalPosiciones];
};
