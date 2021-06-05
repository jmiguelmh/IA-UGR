#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf = 9999999999.0, menosinf = -9999999999.0;

// Constructor
Player::Player(int jug)
{
   jugador_ = jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment &env)
{
   actual_ = env;
}

double Puntuacion(int jugador, const Environment &estado)
{
   double suma = 0;

   for (int i = 0; i < 7; i++)
      for (int j = 0; j < 7; j++)
      {
         if (estado.See_Casilla(i, j) == jugador)
         {
            if (j < 3)
               suma += j;
            else
               suma += (6 - j);
         }
      }

   return suma;
}

// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador)
{
   int ganador = estado.RevisarTablero();

   if (ganador == jugador)
      return 99999999.0; // Gana el jugador que pide la valoracion
   else if (ganador != 0)
      return -99999999.0; // Pierde el jugador que pide la valoracion
   else if (estado.Get_Casillas_Libres() == 0)
      return 0; // Hay un empate global y se ha rellenado completamente el tablero
   else
      return Puntuacion(jugador, estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)

int encuentraConsecutivasVerticales(int fila, int columna, const Environment &estado,int nConsecutivas){
    int cuenta_consec = 0;
    bool sigue = true;

    if(fila + nConsecutivas - 1 < 7){ //Para saber cuando has llegado al tope de las filas
      for (int i = 0; i < nConsecutivas && sigue; i++){
        if (estado.See_Casilla(fila,columna) == estado.See_Casilla((fila+i),columna)){
          cuenta_consec++;
        }else{
          sigue = false;
        }
      }
    }

    if (cuenta_consec == nConsecutivas){
      return 1;
    }else{
      return 0;
    }

}

int encuentraConsecutivasHorizontales(int fila, int columna, const Environment &estado,int nConsecutivas){
  int cuenta_consec = 0;
  bool sigue = true;

  if(columna + nConsecutivas - 1 < 7){ //Para saber si es posible ver las consecutivas sin salirse
    for (int i = 0; i < nConsecutivas && sigue; i++){
      if (estado.See_Casilla(fila,columna) == estado.See_Casilla(fila,(columna+i))){
        cuenta_consec++;
      }else{
          sigue = false;
      }
    }
  }

  if (cuenta_consec == nConsecutivas){
    return 1;
  }else{
    return 0;
  }

}

int encuentraConsecutivasDiagonales(int fila, int columna, const Environment &estado,int nConsecutivas){

  int total = 0;
  int auxfila, auxcolumna;

  auxfila = fila;
  auxcolumna = columna;

  int cuenta_consec = 0;
  bool sigue = true;

  if(fila + nConsecutivas - 1 < 7 && columna + nConsecutivas - 1 < 7){ //Para saber si se puede comprobar las consecutivas
    for (int i = 0; i < nConsecutivas && sigue; i++){
      //ARRIBA A LA DERECHA
      if (estado.See_Casilla(auxfila, auxcolumna) == estado.See_Casilla((auxfila+i),(auxcolumna+i))){
        cuenta_consec++;
      }else{
        sigue = false;
      }
    }
  }

  if (cuenta_consec == nConsecutivas){
    total++;
  }

  cuenta_consec = 0;
  sigue = true;
  auxfila = fila;
  auxcolumna = columna;

  if(fila - nConsecutivas + 1 >= 0 && columna + nConsecutivas - 1 < 7){
    for (int i = 0; i < nConsecutivas && sigue; i++){
      //ABAJO A LA DERECHA
      if (estado.See_Casilla(auxfila, auxcolumna) == estado.See_Casilla((auxfila-i),(auxcolumna+i))){
        cuenta_consec++;
      }else{
        sigue = false;
      }
    }
  }

  if (cuenta_consec == nConsecutivas){
    total++;
  }

  return total;

}

double encuentraConsecutivas(const Environment &estado, int jugador, int nConsecutivas){
    int contador = 0;

    for (int i = 0; i < 7; i++){
      for (int j = 0; j < 7; j++){
        if (estado.See_Casilla(i,j) == jugador){
          //Buscamos consecutivas verticales para [i][j]
          contador += encuentraConsecutivasVerticales(i, j, estado, nConsecutivas);
          //Buscamos consecutivas horizontales para [i][j]
          contador += encuentraConsecutivasHorizontales(i, j, estado, nConsecutivas);
          //Buscamos consecutivas diagonales para [i][j]
          contador += encuentraConsecutivasDiagonales(i, j, estado, nConsecutivas);
        }
      }
    }


    return contador;
}

double Valoracion(const Environment &estado, int jugador)
{
   int rival;
   if(jugador == 1)
      rival = 2;
   else
      rival = 1;

   int tableroEvaluacion[7][7] = {{3,4,5,7,5,4,3},
                                  {4,6,8,10,8,6,4},
                                  {5,8,11,13,11,8,5},
                                  {7,10,13,16,13,10,7},
                                  {5,8,11,13,11,8,5},
                                  {4,6,8,10,8,6,4},
                                  {3,4,5,7,5,4,3},
                                 };

   if(estado.JuegoTerminado())
   {
      if(estado.RevisarTablero() == jugador)
         return masinf;
      else
         return menosinf;
   } else {
      double puntuacion = 0.0, conexionesJugador, conexionesRival;
      int fila, columna;

      for(fila = 0; fila < 7; fila++)
      {
         for(columna = 0; columna < 7; columna++)
         {
            if(estado.See_Casilla(fila,columna) == jugador)
               puntuacion += tableroEvaluacion[fila][columna];
            else if(estado.See_Casilla(fila,columna) == rival)
               puntuacion -= tableroEvaluacion[fila][columna];
         }         
      }

      puntuacion = 0.0;
      conexionesJugador = encuentraConsecutivas(estado, jugador, 2) + encuentraConsecutivas(estado, jugador, 3);
      conexionesRival = encuentraConsecutivas(estado, rival, 2) + encuentraConsecutivas(estado, rival, 3);
      puntuacion += conexionesJugador;
      puntuacion -= conexionesRival;

      return puntuacion;
   }
}

// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j)
{
   j = 0;
   for (int i = 0; i < 8; i++)
   {
      if (aplicables[i])
      {
         opciones[j] = i;
         j++;
      }
   }
}

// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think()
{
   const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
   const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

   Environment::ActionType accion; // acci�n que se va a devolver
   bool aplicables[8];             // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                                   // aplicables[0]==true si PUT1 es aplicable
                                   // aplicables[1]==true si PUT2 es aplicable
                                   // aplicables[2]==true si PUT3 es aplicable
                                   // aplicables[3]==true si PUT4 es aplicable
                                   // aplicables[4]==true si PUT5 es aplicable
                                   // aplicables[5]==true si PUT6 es aplicable
                                   // aplicables[6]==true si PUT7 es aplicable
                                   // aplicables[7]==true si BOOM es aplicable

   double valor;       // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
   double alpha, beta; // Cotas de la poda AlfaBeta

   int n_act; //Acciones posibles en el estado actual

   n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
   int opciones[10];

   // Muestra por la consola las acciones aplicable para el jugador activo
   //actual_.PintaTablero();
   cout << " Acciones aplicables ";
   (jugador_ == 1) ? cout << "Verde: " : cout << "Azul: ";
   for (int t = 0; t < 8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr(static_cast<Environment::ActionType>(t));
   cout << endl;

   //--------------------- COMENTAR Desde aqui
   /*
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }
   */
   //--------------------- COMENTAR Hasta aqui

   //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------

   // Opcion: Poda AlfaBeta
   // NOTA: La parametrizacion es solo orientativa
   alpha = menosinf;
   beta = masinf;
   valor = podaAlfaBeta(actual_, jugador_, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
   cout << "Valor podaAlfaBeta: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

   return accion;
}

void CalculaMax(double &max, double valor, Environment::ActionType &accion, int mejorMovimiento)
{
   if(valor > max)
   {
      max = valor;
      accion = static_cast< Environment::ActionType > (mejorMovimiento);
   }
}

void CalculaMin(double &min, double valor, Environment::ActionType &accion, int mejorMovimiento)
{
   if(valor < min)
   {
      min = valor;
      accion = static_cast< Environment::ActionType > (mejorMovimiento);
   }
}

double Player::podaAlfaBeta(const Environment &actual, int jugador, int profundidad, Environment::ActionType &accion, double alpha, double beta)
{
   bool movimientos[8];
   int movimientosPosibles = actual.possible_actions(movimientos);
   Environment::ActionType accionAnterior;

   if(actual.JuegoTerminado() || movimientosPosibles == 0 || profundidad == 0)
   {
      return Valoracion(actual,jugador);
   } else {
      int mejorMovimiento = -1;
      Environment tablero = actual.GenerateNextMove(mejorMovimiento);

      if(jugador == actual.JugadorActivo())
      {
         int contador = 0;

         while(contador < movimientosPosibles && beta > alpha)
         {
            contador++;
            double valor = podaAlfaBeta(tablero, jugador, profundidad-1, accionAnterior, alpha, beta);
            CalculaMax(alpha, valor, accion, mejorMovimiento);
            tablero = actual.GenerateNextMove(mejorMovimiento);
         }

         return alpha;
      } else {
         int contador = 0;

         while(contador < movimientosPosibles && beta > alpha)
         {
            contador++;
            double valor = podaAlfaBeta(tablero, jugador, profundidad-1, accionAnterior, alpha, beta);
            CalculaMin(beta, valor, accion, mejorMovimiento);
            tablero = actual.GenerateNextMove(mejorMovimiento);
         }

         return beta;
      }
   }
}

