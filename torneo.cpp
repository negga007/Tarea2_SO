#include <vector>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include "tablita.hpp"
#include "mesa.hpp"
#include "apagado.hpp"
#include "estado.hpp"
#include "config.hpp"
#include "puma.hpp"
#include "synchro.hpp"
#include "can.hpp"
#include "juez.hpp"
using namespace std;

static sem_t cupos; 

// lo q recibe cada hilo partida
struct rutPartida {
    int rutMesa;
    const GRafo *tablero;
    const GRafoCapturas *mapeoCapturas;
    const configuraxion *config;
};

// una partida
void *hiloDeLamesa(void *datosDlaMesa){
    rutPartida &datitos = *(rutPartida *)datosDlaMesa;
    semEsperar(&cupos);

    Mesa mesa;
    mesa.idMesa = datitos.rutMesa;
    mesa.tablero = datitos.tablero;
    mesa.mapeoCapturas = datitos.mapeoCapturas;
    mesa.config = datitos.config;
    mesa.iniciada = false;
    pthread_mutex_init(&mesa.mutexInit, NULL);
    pthread_cond_init(&mesa.condInit, NULL);

    pthread_t hiloDelJuez;
    pthread_create(&hiloDelJuez, NULL, hiloJuez, &mesa);

    // esperar que el juez termine de inicializar la mesa
    {
        Guard guardia(&mesa.mutexInit);
        while(!mesa.iniciada){
            pthread_cond_wait(&mesa.condInit, &mesa.mutexInit);
        }
    }

    // si llego ctrl+c antes de jugar, terminamos al juez y salimos
    if(!meterMesa(&mesa)){
        {
            Guard guardia(&mesa.mutexAviso);
            mesa.terminada = true;
            pthread_cond_broadcast(&mesa.avisoAlJuez);
        }
        pthread_join(hiloDelJuez, NULL);
        destruirMesa(mesa);
        pthread_mutex_destroy(&mesa.mutexInit);
        pthread_cond_destroy(&mesa.condInit);
        semAvisar(&cupos);
        return NULL;
    }

    pthread_t hiloDelPuma;
    pthread_t hilosCanes[TotalPerros];
    rutCan rutas[TotalPerros];

    pthread_create(&hiloDelPuma, NULL, hiloPuma, &mesa);

    for(int idCan = 0; idCan < TotalPerros; idCan++){
        rutas[idCan].mesa = &mesa;
        rutas[idCan].idCan = idCan;
        pthread_create(&hilosCanes[idCan], NULL, hiloCan, &rutas[idCan]);
    }

    pthread_join(hiloDelJuez, NULL);
    pthread_join(hiloDelPuma, NULL);

    for(auto &hilo : hilosCanes){
        pthread_join(hilo, NULL);
    }

    sacarMesa(&mesa);
    destruirMesa(mesa);
    pthread_mutex_destroy(&mesa.mutexInit);
    pthread_cond_destroy(&mesa.condInit);
    semAvisar(&cupos);
    return NULL;
}

int main(int argc, char *argv[]){
    //ommmmm recordatorio ./torneo N M [config.txt]
    if(argc < 3 || argc > 4){
        cerr << "Uso: ./torneo N M [config.txt]" << endl;
        return 1;
    }
    int numeroN = atoi(argv[1]); //cuantos jugadores llegan
    int mesasM = atoi(argv[2]);  //max de mesas a la vez

    //si no son numeros atoi da 0
    if(numeroN <= 0 || mesasM <= 0){
        cerr << "N y M tienen q ser numeros mayores a 0" << endl;
        return 1;
    }
    
    //valores por defecto, si viene config los pisa
    configuraxion config;
    if(argc == 4){
        if(!leerConfig(argv[3], config)){
            return 1; //leerConfig ya aviso cual fue el error
        }
    }

    //contriir tablero
    GRafo tablero;
    contrsuirGrafo(tablero);
    //craftear mapa de capturas
    GRafoCapturas mapeoCapturas;
    construirCapturas(mapeoCapturas);

    cout << numeroN << " jugadores, " << mesasM << " mesas" << endl;
    
    
    int partidas = numeroN / 2; //2 jugadore

    if(numeroN % 2 != 0){
        cout << "sobra 1 jugador, no tiene con quien jugar" << endl;
    }
    if(partidas == 0){
        return 0;
    }

    semIniciar(&cupos, mesasM);

    sigset_t bloqueadas;
    sigemptyset(&bloqueadas);
    sigaddset(&bloqueadas, SIGINT);
    pthread_sigmask(SIG_BLOCK, &bloqueadas, NULL);

    pthread_t hiloCtrlC;
    pthread_create(&hiloCtrlC, NULL, hiloSigint, NULL);

    vector<pthread_t> hilosPartida(partidas);
    vector<rutPartida> caminosDelaPartida(partidas);

    for(int rutMesa = 0; rutMesa < partidas; rutMesa++){
        caminosDelaPartida[rutMesa].rutMesa = rutMesa;
        caminosDelaPartida[rutMesa].tablero = &tablero;
        caminosDelaPartida[rutMesa].mapeoCapturas = &mapeoCapturas;
        caminosDelaPartida[rutMesa].config = &config;
        pthread_create(&hilosPartida[rutMesa], NULL, hiloDeLamesa, &caminosDelaPartida[rutMesa]);
    }

    for(auto &hilo : hilosPartida){
        pthread_join(hilo, NULL);
    }

    pthread_kill(hiloCtrlC, SIGINT); 
    pthread_join(hiloCtrlC, NULL);

    semDestruir(&cupos);

    return 0;
}