#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

struct LaJugada{
  double valor;
  int la_accion;
};

class Player{
    public:
      Player(int jug);
      Environment::ActionType Think();
      void Perceive(const Environment &env);

      /**
       * METODO DE PODA ALFA BETA. LA PODA SE REALIZA CUANDO BETA > ALFA
       * @param actual, es la situacion del juego actual (de la que se generarán tantos hijos como sea posible para su valoracion)
       * @param jugador, AlfaBeta recibe el jugador de la clase Player. Siempre valdrá 1 si el jugador es VERDE, 2 si es AZUL
       * @param profundidad, es el máximo de profundidad a la que el algoritmo analizará. Comienza en el máximo y va disminuyendo a 0
       * @param accion, va guardando siempre el mejor movimiento a razón de la buena o mala valoracion de la heurística
       * @param alpha, valor de ganancia máxima en un nodo MAX.
       * @param beta, valor de perdida mínima en un nodo MIN.
       * @param primerAnalisis, si es TRUE la primera comprobacion antes de pasar al algoritmo es ver si haga al explotar una bomba (de tenerla)
       * @return valor del algoritmo
       */ 
      double poda_AlfaBeta (const Environment & actual, int jugador, int profundidad, Environment::ActionType & accion, double alpha, double beta, bool& primerAnalisis);
      
    private:
      int jugador_;
      Environment actual_;

};
#endif