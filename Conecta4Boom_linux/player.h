#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

class Player
{
public:
  Player(int jug);
  Environment::ActionType Think();
  void Perceive(const Environment &env);
  double podaAlfaBeta(const Environment &actual, int jugador, int profundidad, Environment::ActionType &accion, double alpha, double beta);

private:
  int jugador_;
  Environment actual_;
};
#endif
