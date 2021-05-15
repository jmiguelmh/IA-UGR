#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

// Funciones auxiliares para pathfinding_CosteUniforme
// Calcula el coste de la casilla cuando se produce un actFORWARD
void ComportamientoJugador::CalcularCosteCasillaAvanzar(estado &st, const vector<vector<unsigned char>> &map)
{
	switch (map[st.fila][st.columna])
	{
	case 'A':
		if (bikini)
			st.costeCasilla = 10;
		else
			st.costeCasilla = 200;
		break;

	case 'B':
		if (zapatillas)
			st.costeCasilla = 15;
		else
			st.costeCasilla = 100;
		break;

	case 'T':
		st.costeCasilla = 2;
		break;

	default:
		st.costeCasilla = 1;
		break;
	}
}

// Calcula el coste de la casilla cuando se produce un actTURN_L o actTURN_R
void ComportamientoJugador::CalcularCosteCasillaGirar(estado &st, const vector<vector<unsigned char>> &map)
{
	switch (map[st.fila][st.columna])
	{
	case 'A':
		if (bikini)
			st.costeCasilla = 5;
		else
			st.costeCasilla = 500;
		break;

	case 'B':
		if (zapatillas)
			st.costeCasilla = 1;
		else
			st.costeCasilla = 3;
		break;

	case 'T':
		st.costeCasilla = 2;
		break;

	default:
		st.costeCasilla = 1;
		break;
	}
}

// Calcula el coste uniforme con el coste de la casilla y el coste uniforme del padre
void CalcularCosteUniforme(estado &st, const estado &padre)
{
	st.costeUniforme = st.costeCasilla + padre.costeUniforme;
}

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	actual.fila = sensores.posF;
	actual.columna = sensores.posC;
	actual.orientacion = sensores.sentido;

	if (mapaResultado[actual.fila][actual.columna] == 'K' && !bikini)
		bikini = true;
	else if (mapaResultado[actual.fila][actual.columna] == 'D' && !zapatillas)
		zapatillas = true;

	objetivos.clear();
	for (int i = 0; i < sensores.num_destinos; i++)
	{
		estado aux;
		aux.fila = sensores.destino[2 * i];
		aux.columna = sensores.destino[2 * i + 1];
		objetivos.push_back(aux);
	}

	if (!hayPlan)
	{
		hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan);
	}

	Action sigAccion;
	if (plan.size() > 0)
	{
		sigAccion = plan.front();
		plan.erase(plan.begin());
	}
	else
	{
		cout << "No se pudo encontrar un plan";
	}

	return sigAccion;
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan)
{
	switch (level)
	{
	case 0:
		cout << "Demo\n";
		estado un_objetivo;
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Profundidad(origen, un_objetivo, plan);
		break;

	case 1:
		cout << "Optimo numero de acciones\n";
		// Incluir aqui la llamada al busqueda en anchura
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Anchura(origen, un_objetivo, plan);
		break;
	case 2:
		cout << "Optimo en coste 1 Objetivo\n";
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Coste_Uniforme(origen, un_objetivo, plan);
		break;
	case 3:
		cout << "Optimo en coste 3 Objetivos\n";
		// Incluir aqui la llamada al algoritmo de busqueda para 3 objetivos
		cout << "No implementado aun\n";
		break;
	case 4:
		cout << "Algoritmo de busqueda usado en el reto\n";
		// Incluir aqui la llamada al algoritmo de busqueda usado en el nivel 2
		cout << "No implementado aun\n";
		break;
	}
	return false;
}

//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')
		return true;
	else
		return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st)
{
	int fil = st.fila, col = st.columna;

	// calculo cual es la casilla de delante del agente
	switch (st.orientacion)
	{
	case 0:
		fil--;
		break;
	case 1:
		col++;
		break;
	case 2:
		fil++;
		break;
	case 3:
		col--;
		break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil < 0 or fil >= mapaResultado.size())
		return true;
	if (col < 0 or col >= mapaResultado[0].size())
		return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col]))
	{
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
		st.fila = fil;
		st.columna = col;
		return false;
	}
	else
	{
		return true;
	}
}

struct nodo
{
	estado st;
	list<Action> secuencia;
};

// Struct que permite comparar los costes uniformes en la priority queue
struct comparaCosteUniforme
{
	bool operator()(const nodo &n1, const nodo &n2)
	{
		return n1.st.costeUniforme > n2.st.costeUniforme;
	}
};

struct ComparaEstados
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};

// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan)
{
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 1) % 4;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 3) % 4;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}


// Algoritmo de búsqueda en anchura, es exactamente igual al de profundidad a diferencia que se ha utilizado una queue en vez de stack
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan)
{
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados;
	queue<nodo> Abiertos;

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 1) % 4;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 3) % 4;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		if (!Abiertos.empty())
		{
			current = Abiertos.front();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

// Algoritmo de búsqueda de coste uniforme en el que se ha utilizado una priority queue que mantiene ordenados los nodos segun su coste uniforme
// Dese cuenta que para avanzar y girar se utiliza las distintas funciones de calculo de coste de casilla
bool ComportamientoJugador::pathFinding_Coste_Uniforme(const estado &origen, const estado &destino, list<Action> &plan)
{
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados;
	priority_queue<nodo, vector<nodo>, comparaCosteUniforme> Abiertos;

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	CalcularCosteCasillaAvanzar(current.st, mapaResultado);
	current.st.costeUniforme = current.st.costeCasilla;

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		if (Cerrados.find(current.st) == Cerrados.end())
			Cerrados.insert(current.st);

		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 1) % 4;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			CalcularCosteCasillaGirar(hijoTurnR.st, mapaResultado);
			CalcularCosteUniforme(hijoTurnR.st, current.st);
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 3) % 4;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			CalcularCosteCasillaGirar(hijoTurnL.st, mapaResultado);
			CalcularCosteUniforme(hijoTurnL.st, current.st);
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				CalcularCosteCasillaAvanzar(hijoForward.st, mapaResultado);
				CalcularCosteUniforme(hijoForward.st, current.st);
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

void ComportamientoJugador::PintaPlan(list<Action> plan)
{
	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			cout << "A ";
		}
		else if (*it == actTURN_R)
		{
			cout << "D ";
		}
		else if (*it == actTURN_L)
		{
			cout << "I ";
		}
		else
		{
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

void AnularMatriz(vector<vector<unsigned char>> &m)
{
	for (int i = 0; i < m[0].size(); i++)
	{
		for (int j = 0; j < m.size(); j++)
		{
			m[i][j] = 0;
		}
	}
}

void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			switch (cst.orientacion)
			{
			case 0:
				cst.fila--;
				break;
			case 1:
				cst.columna++;
				break;
			case 2:
				cst.fila++;
				break;
			case 3:
				cst.columna--;
				break;
			}
			mapaConPlan[cst.fila][cst.columna] = 1;
		}
		else if (*it == actTURN_R)
		{
			cst.orientacion = (cst.orientacion + 1) % 4;
		}
		else
		{
			cst.orientacion = (cst.orientacion + 3) % 4;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}