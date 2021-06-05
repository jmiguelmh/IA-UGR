#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){

    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar


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

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Valoracion(const Environment &estado, int jugador){

  /*
  Metodo See_Casilla(fila, columna);
  Devuelve lo que hay en el tablero en la fila, columna: 0 vacia, 1 jugador1, 2 jugador2.
  La bomba, el valor 4 es una casilla con una bomba verde
  la bomba, el valor 5 es una casilla con la bomba azul
  */

  int rival = 1;

  if (jugador == 1){
      rival = 2;
  }

  //Obtener la puntuacion del jugador y el rival

  double rival_cuatros, rival_tres, rival_dos, yo_cuatros, yo_tres, yo_dos;
  double yo, oponente;

  //encuentraConsecutivas(tablero, jugador, nConsecutivas (a buscar))
  //La valoracion se hace para cada casilla del tablero
  rival_cuatros = encuentraConsecutivas(estado, rival, 4);
  rival_tres = encuentraConsecutivas(estado, rival, 3);
  rival_dos = encuentraConsecutivas(estado, rival, 2);

  yo_cuatros = encuentraConsecutivas(estado, jugador, 4);
  yo_tres = encuentraConsecutivas(estado, jugador, 3);
  yo_dos = encuentraConsecutivas(estado,jugador,2);

  if (estado.JuegoTerminado()){
    if(estado.RevisarTablero2() == jugador){
      return masinf;
    }else{
      return menosinf;
    }

  }else{

    yo = (yo_cuatros*10 + yo_tres*1 + yo_dos*10);
    //Cuantas mas conecte el rival, es mejor para mi
    oponente = (rival_cuatros*10 + rival_tres*1 + rival_dos*10);

    return (yo - oponente); //Correcto

  }

}

// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}

// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // accion que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha, beta; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual

    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;

/*
    //--------------------- COMENTAR Desde aqui
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
    //--------------------- COMENTAR Hasta aqui
*/
    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


  // Opcion: Poda AlfaBeta
  // NOTA: La parametrizacion es solo orientativa

  alpha = menosinf;
  beta = masinf;
  bool primerAnalisis = true;

  
  valor = poda_AlfaBeta(actual_, jugador_, PROFUNDIDAD_ALFABETA, accion, alpha, beta, primerAnalisis);
  cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;
  return accion;
}


void BuscaMax (double & valorMax, double valor2, Environment::ActionType & accion, int mejor_accion){
    if(valor2 > valorMax){
        valorMax = valor2;
        accion = static_cast< Environment::ActionType > (mejor_accion);
    }

}

void BuscaMin (double & valorMin, double valor2, Environment::ActionType & accion, int mejor_accion){
    if(valor2 < valorMin){
        valorMin = valor2;
        accion = static_cast< Environment::ActionType > (mejor_accion);
    }

}

double Player::poda_AlfaBeta (const Environment & actual, int jugador, int profundidad, Environment::ActionType & accion, double alpha, double beta, bool& primerAnalisis){

  if(primerAnalisis){
    primerAnalisis = false;
    if(actual.Have_BOOM(jugador)){

      int bomba = 7;
      Environment explotar = actual.GenerateNextMove(bomba);

      if( explotar.JuegoTerminado()){
        //Si al explotar se gana, se explota la bomba
        if(explotar.RevisarTablero2() == jugador){
          accion = static_cast< Environment::ActionType > (bomba);
          return masinf;
        }

      }
    }
  }

  bool opciones[8];
  int hijosPosibles = actual.possible_actions(opciones);
  Environment::ActionType accion_anterior;

  //Cuando el juego termina o ya se ha disminuido toda la profundidad establecida
  if (actual.JuegoTerminado() || profundidad == 0 || hijosPosibles == 0){
	//cout << "Valoracion:"  << ValoracionTest(actual,jugador) << endl;
    return ValoracionTest(actual, jugador); //Se valora el nodo

  }else{ //Se deben generar hijos
    int mejor_accion = -1; //La última accion evaluada

    //Aplica la SIGUIENTE accion que se puede hacer. Si pasas un -1, te devuelve la siguiente que pueda aplicar, observando desde PUT1 A BOOM.
    //Si no se puede hacer ninguna accion mas, devuelve el propio nodo que hace la llamada.
    Environment nodo = actual.GenerateNextMove(mejor_accion); //Se genera un hijo

    if(jugador == actual.JugadorActivo()){ //Nodo MAX. O sea, es mi turno

      int cont = 0;
      //Generará hijos hasta que cumpla la cantidad de hijos que puede generar el nodo actual que llega a PodaAlfaBeta
      //O hasta que se cumpla la condicion de PODA
      while (cont < hijosPosibles && beta > alpha){
        cont++;
        BuscaMax(alpha, (poda_AlfaBeta(nodo, jugador, profundidad-1 , accion_anterior, alpha, beta, primerAnalisis)), accion, mejor_accion);
        nodo = actual.GenerateNextMove(mejor_accion); //Se genera un hijo
      }
	//cout << "alpha: " << alpha << endl;
      return alpha;

    }else{

      int cont = 0;
      //Hasta que no haya más hijos que generar o se cumpla la condicion de Poda
      while (cont < hijosPosibles && beta > alpha){

        cont++;
        BuscaMin(beta, (poda_AlfaBeta(nodo, jugador, profundidad-1 , accion_anterior, alpha, beta, primerAnalisis)), accion, mejor_accion);
        nodo = actual.GenerateNextMove(mejor_accion); //Se genera un hijo. MejorAccion se actualiza al movimiento que se ha realizado

      }
	//cout << "beta: " << beta << endl;
      return beta;

    }

  }

}