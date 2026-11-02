#include "tablita.hpp"

void contrsuirGrafo(GRafo &grafo){
    for(int y = 0; y < TamannoTabla; y++){
        for(int x = 0; x < TamannoTabla; x++){
            int id = AsignadorDID(x,y);
            if(y-1 >= 0){ //esta cosa revisa si es valida la pos en el tablero para aarmar un grafo
                //de como si se puede mover de un lugar a otro, tipo ya el 00 esta conectado con el
                // 01 y el 10 okey entonces hacemos un vector con en cada posicion de este
                //una matriz con los que son vecinos de este
                grafo.vecinos[id].push_back(AsignadorDID(x,y-1));
            }
            if(y+1 < TamannoTabla){
                grafo.vecinos[id].push_back(AsignadorDID(x,y+1));
            }
            if(x-1 >= 0){
                grafo.vecinos[id].push_back(AsignadorDID(x-1,y));
            }
            if(x+1 < TamannoTabla){
                grafo.vecinos[id].push_back(AsignadorDID(x+1,y));
            }
        }
    }

        // trono arriba eli d 27
    grafo.vecinos[Trono_up].push_back(Trono_izq);
    grafo.vecinos[Trono_up].push_back(Trono_derecha);
    grafo.vecinos[Trono_up].push_back(AsignadorDID(2, 4));

    // trono izquierda el id  25
    grafo.vecinos[Trono_izq].push_back(Trono_up);
    grafo.vecinos[Trono_izq].push_back(Trono_derecha);
    grafo.vecinos[Trono_izq].push_back(AsignadorDID(1, 4));
    grafo.vecinos[Trono_izq].push_back(AsignadorDID(0, 4));

    //  trono derecha el id 26
    grafo.vecinos[Trono_derecha].push_back(Trono_up);
    grafo.vecinos[Trono_derecha].push_back(Trono_izq);
    grafo.vecinos[Trono_derecha].push_back(AsignadorDID(3, 4));
    grafo.vecinos[Trono_derecha].push_back(AsignadorDID(4, 4));

         // conexion con el grid
    grafo.vecinos[AsignadorDID(2, 4)].push_back(Trono_up);
    grafo.vecinos[AsignadorDID(1, 4)].push_back(Trono_izq);
    grafo.vecinos[AsignadorDID(0, 4)].push_back(Trono_izq);
    grafo.vecinos[AsignadorDID(3, 4)].push_back(Trono_derecha);
    grafo.vecinos[AsignadorDID(4, 4)].push_back(Trono_derecha);
}

//CONstruir las posiciones que el puma puede saltar
void construirCapturas(GRafoCapturas &grafoCapturas){
    for(int y = 0; y < TamannoTabla; y++){
        for(int x = 0; x < TamannoTabla; x++){
            int id = AsignadorDID(x, y);

            if(y - 2 >= 0){ //abajo
                int sobre = AsignadorDID(x, y - 1);
                int destino = AsignadorDID(x, y - 2);
                Capturas salto;
                salto.posCapturada = sobre;
                salto.aterrizaje = destino;
                grafoCapturas.Captura[id].push_back(salto);
            }

            // arriva
            if(y + 2 < TamannoTabla){
                int sobre = AsignadorDID(x, y + 1);
                int destino = AsignadorDID(x, y + 2);
                Capturas salto;
                salto.posCapturada = sobre;
                salto.aterrizaje = destino;
                grafoCapturas.Captura[id].push_back(salto);
            }

            // izq
            if(x - 2 >= 0){
                int sobre = AsignadorDID(x - 1, y);
                int destino = AsignadorDID(x - 2, y);
                Capturas salto;
                salto.posCapturada = sobre;
                salto.aterrizaje = destino;
                grafoCapturas.Captura[id].push_back(salto);
            }

            // der
            if(x + 2 < TamannoTabla){
                int sobre = AsignadorDID(x + 1, y);
                int destino = AsignadorDID(x + 2, y);
                Capturas salto;
                salto.posCapturada = sobre;
                salto.aterrizaje = destino;
                grafoCapturas.Captura[id].push_back(salto);
            }
        }
    }

    //trono
    int sobre;
    int destino;
    Capturas salto;

    // 17 a top
    sobre = AsignadorDID(2, 4);
    destino = Trono_up;
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[AsignadorDID(2, 3)].push_back(salto);
    //top a 17
    sobre = AsignadorDID(2, 4);
    destino = AsignadorDID(2, 3);
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[Trono_up].push_back(salto);

    //top a 20
    sobre = Trono_izq;
    destino = AsignadorDID(0, 4);
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[Trono_up].push_back(salto);
    //20 a top
    sobre = Trono_izq;
    destino = Trono_up;
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[AsignadorDID(0, 4)].push_back(salto);

    // top a 24
    sobre = Trono_derecha;
    destino = AsignadorDID(4, 4);
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[Trono_up].push_back(salto);
    //24 a top
    sobre = Trono_derecha;
    destino = Trono_up;
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[AsignadorDID(4, 4)].push_back(salto);



    // 16 a izq
    sobre = AsignadorDID(1, 4);
    destino = Trono_izq;
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[AsignadorDID(1, 3)].push_back(salto);
    //izq a 16
    sobre = AsignadorDID(1, 4);
    destino = AsignadorDID(1, 3);
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[Trono_izq].push_back(salto);

    // 18 a der
    sobre = AsignadorDID(3, 4);
    destino = Trono_derecha;
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[AsignadorDID(3, 3)].push_back(salto);
    //der a 18
    sobre = AsignadorDID(3, 4);
    destino = AsignadorDID(3, 3);
    salto.posCapturada = sobre;
    salto.aterrizaje = destino;
    grafoCapturas.Captura[Trono_derecha].push_back(salto);
    
}