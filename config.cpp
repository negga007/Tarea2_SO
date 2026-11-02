#include <fstream>   
#include <sstream>   //lector de lineas we recordar
#include <string>
#include <iostream>
#include "config.hpp"
using namespace std;

//secciom de lectura
bool leerConfig(const string &ruta, configuraxion &config){
    //abrimos el archivo
    ifstream archivo(ruta);
    if(!archivo.is_open()){
        cerr << "No se pudo abrir el config: " << ruta << endl;
        return false;
    }

    string carlitosLector; //lector linea a linea
    int lineaQvamos = 0; 

    while(getline(archivo, carlitosLector)){
        lineaQvamos++;

        //esto separa por espacios 
        istringstream partes(carlitosLector);
        string pedazito;
        partes >> pedazito; 

        //lineas vacias o comentarios (con #) las saltamos
        if(pedazito.empty() || pedazito[0] == '#'){
            continue;
        }

        bool linesTabn = true; //rellenado basico
        if(pedazito == "SEED"){
            partes >> config.semilla;
        }else if(pedazito == "DELAY_MIN"){
            partes >> config.retardoMin;
        }else if(pedazito == "DELAY_MAX"){
            partes >> config.retardoMax;
        }else if(pedazito == "PUMA_INIT"){
            string dondeEstaPuma;
            partes >> dondeEstaPuma;
            //puede ser el nombre de un trono...
            if(dondeEstaPuma == "TRONO_TOP"){
                config.posPuma = Trono_up;
            }else if(dondeEstaPuma == "TRONO_LEFT"){
                config.posPuma = Trono_izq;
            }else if(dondeEstaPuma == "TRONO_RIGHT"){
                config.posPuma = Trono_derecha;
            }else{
                //coord x e y
                int x = -1;
                int y = -1; // en -1 pk si lo ponen mal que avise esto
                istringstream numeroX(dondeEstaPuma);
                numeroX >> x;
                partes >> y;
                
                if(numeroX.fail() || x < 0 || x >= TamannoTabla || y < 0 || y >= TamannoTabla){
                    //si no esta en la grilla bien puesto
                    linesTabn = false;
                }else{
                    config.posPuma = AsignadorDID(x, y); 
                }
            }
        }else if(pedazito == "DOG"){
            int idCan = -1;
            int x = -1;
            int y = -1;

            partes >> idCan >> x >> y;
            
            //el perro tiene que ser del 0 al 11 y la posicion dentro del tablero,,,
            if(idCan < 0 || idCan >= TotalPerros || x < 0 || x >= TamannoTabla || y < 0 || y >= TamannoTabla){
                linesTabn = false;
            }else{
                config.posPerros[idCan] = AsignadorDID(x, y);
            }
        }else{
            linesTabn = false; //no es ninguna
        }

        //caso extremo d q algo fallo al leer
        if(partes.fail() || !linesTabn){
            cerr << "Config: linea " << lineaQvamos << " invalida: " << carlitosLector << endl;
            return false;
        }
    }

    //el retardo minimo no puede ser mayor que el maximo, logica para q no explote
    if(config.retardoMin > config.retardoMax){
        cerr << "Config: DELAY_MIN no puede ser mayor que DELAY_MAX" << endl;
        return false;
    }
    return true;
}