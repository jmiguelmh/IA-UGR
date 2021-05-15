#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado
{
  int fila;
  int columna;
  int orientacion;
  int costeCasilla;
  int costeUniforme;
};

class ComportamientoJugador : public Comportamiento
{
public:
  ComportamientoJugador(unsigned int size) : Comportamiento(size)
  {
    hayPlan = false;
    bikini = false;
    zapatillas = false;
  }
  ComportamientoJugador(std::vector<std::vector<unsigned char>> mapaR) : Comportamiento(mapaR)
  {
    hayPlan = false;
    bikini = false;
    zapatillas = false;
  }
  ComportamientoJugador(const ComportamientoJugador &comport) : Comportamiento(comport) {}
  ~ComportamientoJugador() {}

  Action think(Sensores sensores);
  int interact(Action accion, int valor);
  void VisualizaPlan(const estado &st, const list<Action> &plan);
  ComportamientoJugador *clone() { return new ComportamientoJugador(*this); }

private:
  estado actual;
  list<estado> objetivos;
  list<Action> plan;

  //Variables de estado añadidas
  bool hayPlan;
  bool bikini;
  bool zapatillas;

  bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan);
  bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
  bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
  bool pathFinding_Coste_Uniforme(const estado &origen, const estado &destino, list<Action> &plan);

  void PintaPlan(list<Action> plan);
  bool HayObstaculoDelante(estado &st);
  void CalcularCosteCasillaAvanzar(estado &st, const vector<vector<unsigned char>> &map);
  void CalcularCosteCasillaGirar(estado &st, const vector<vector<unsigned char>> &map);
};

#endif
