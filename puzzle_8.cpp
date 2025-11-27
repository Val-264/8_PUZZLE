#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include <functional>  // Necesario para std::function

using namespace std;

#define TAM 50
const int tiempo_espera = 1000;

//-----------CLASE PARA MANEJO DE PUNTAJES-----------
class Clase_Usuario { 
    private:
        struct Datos_Usuario {
            char nombreUsuario[TAM];
            int puntuacion;
            struct Fecha {
                int dd;
                int mm;
                int aa;
            }; 
            Fecha dia;
        };
        
    public:
        Clase_Usuario(){}
        
        void guardarNombrePuntaje(char nombre[TAM], int puntaje) {
            Datos_Usuario nuevo, existente;
            fstream puntajes;
            bool existe = false;
            streampos posicion;

            puntajes.open("puntajes.dat", ios::binary | ios::in | ios::out);

            if(!puntajes) {
                puntajes.open("puntajes.dat", ios::binary | ios::out);
                puntajes.close();
                puntajes.open("puntajes.dat", ios::binary | ios::in | ios::out);
                
                if(!puntajes) {
                    cerr << "\nNo se pudo abrir el archivo para guardar puntajes\n";
                    return;
                }
            }

            while (puntajes.read(reinterpret_cast<char*>(&existente), sizeof(Datos_Usuario))) {
                if (strcmp(existente.nombreUsuario, nombre) == 0) {
                    existe = true;
                    posicion = puntajes.tellg() - streampos(sizeof(Datos_Usuario));
                    break;
                }
            }

            if (existe) {
                existente.puntuacion += puntaje;
                time_t hoy = time(0);
                tm * timeinfo = localtime(&hoy);
                existente.dia.dd = timeinfo->tm_mday; 
                existente.dia.mm = timeinfo->tm_mon + 1; 
                existente.dia.aa = 1900 + timeinfo->tm_year;
                
                puntajes.seekp(posicion);
                puntajes.write(reinterpret_cast<char*>(&existente), sizeof(Datos_Usuario));
            } else {
                strcpy(nuevo.nombreUsuario, nombre);
                nuevo.puntuacion = puntaje;
                time_t hoy = time(0);
                tm * timeinfo = localtime(&hoy);
                nuevo.dia.dd = timeinfo->tm_mday;
                nuevo.dia.mm = timeinfo->tm_mon + 1;
                nuevo.dia.aa = 1900 + timeinfo->tm_year;
                
                puntajes.clear();
                puntajes.seekp(0, ios::end);
                puntajes.write(reinterpret_cast<char*>(&nuevo), sizeof(Datos_Usuario));
            }
            puntajes.close();
        }

        void mostrarPuntajes() {
            Datos_Usuario guardado;
            ifstream puntajes;
            puntajes.open("puntajes.dat", ios::binary | ios::in);

            if (!puntajes) {
                cout << "\nNo hay puntajes guardados aún.\n";
                return;
            }

            puntajes.seekg(0, ios::end);
            if (puntajes.tellg() == 0) {
                cout << "\nNo hay puntajes guardados aún.\n";
                puntajes.close();
                return;
            }
            puntajes.seekg(0, ios::beg);

            cout << "\n                PUZZLE 8         \n\n";
            cout << "------------------PUNTAJES------------------\n";
            cout << "Nombre\t\tPuntaje\tFecha\n";
            cout << "--------------------------------------------\n";

            while (puntajes.read(reinterpret_cast<char*>(&guardado), sizeof(Datos_Usuario))) {
                cout << guardado.nombreUsuario << "\t\t" 
                     << guardado.puntuacion << "\t"  
                     << guardado.dia.dd << "/" << guardado.dia.mm << "/" << guardado.dia.aa << endl;
            }
            puntajes.close();
            cout << "\n--------------------------------------------\n";
        }
};

//-----------CLASE PARA RESOLVER CON A*-----------
class A_star {
    private:
        struct Nodo {
            vector<vector<int>> estado;
            int costo_g;
            int costo_h;
            int costo_f;
            Nodo* padre;
            
            Nodo(vector<vector<int>> e, int g, int h, Nodo* p = nullptr) 
                : estado(e), costo_g(g), costo_h(h), padre(p) {
                costo_f = costo_g + costo_h;
            }
        };
        
        // Función de comparación para la priority_queue
        struct CompararNodo {
            bool operator()(const Nodo* a, const Nodo* b) const {
                return a->costo_f > b->costo_f;
            }
        };
        
        vector<vector<int>> estadoFinal;
        
        int calcularHeuristica(const vector<vector<int>>& estado) {
            int distancia = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int valor = estado[i][j];
                    if (valor != 0) {
                        // Encontrar la posición objetivo de este valor
                        for (int k = 0; k < 3; k++) {
                            for (int l = 0; l < 3; l++) {
                                if (estadoFinal[k][l] == valor) {
                                    distancia += abs(i - k) + abs(j - l);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            return distancia;
        }
        
        pair<int, int> encontrarCero(const vector<vector<int>>& estado) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (estado[i][j] == 0) return {i, j};
                }
            }
            return {-1, -1};
        }
        
        vector<vector<int>> copiarEstado(const vector<vector<int>>& estado) {
            vector<vector<int>> nuevo(3, vector<int>(3));
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    nuevo[i][j] = estado[i][j];
                }
            }
            return nuevo;
        }
        
        string estadoAString(const vector<vector<int>>& estado) {
            string s;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    s += to_string(estado[i][j]);
                }
            }
            return s;
        }
        
    public:
        A_star(vector<vector<int>> final) : estadoFinal(final) {}
        
        vector<vector<vector<int>>> resolver(vector<vector<int>> estadoInicial) {
            priority_queue<Nodo*, vector<Nodo*>, CompararNodo> frontera;
            set<string> visitados;
            
            int h_inicial = calcularHeuristica(estadoInicial);
            Nodo* nodoInicial = new Nodo(estadoInicial, 0, h_inicial);
            frontera.push(nodoInicial);
            visitados.insert(estadoAString(estadoInicial));
            
            while (!frontera.empty()) {
                Nodo* actual = frontera.top();
                frontera.pop();
                
                if (actual->estado == estadoFinal) {
                    vector<vector<vector<int>>> camino;
                    Nodo* temp = actual;
                    while (temp != nullptr) {
                        camino.push_back(temp->estado);
                        temp = temp->padre;
                    }
                    reverse(camino.begin(), camino.end());
                    
                    // Limpiar memoria
                    while (!frontera.empty()) {
                        delete frontera.top();
                        frontera.pop();
                    }
                    return camino;
                }
                
                auto posCero = encontrarCero(actual->estado);
                int i = posCero.first, j = posCero.second;
                
                vector<pair<int, int>> movimientos = {{-1,0}, {1,0}, {0,-1}, {0,1}};
                
                for (auto mov : movimientos) {
                    int ni = i + mov.first, nj = j + mov.second;
                    if (ni >= 0 && ni < 3 && nj >= 0 && nj < 3) {
                        vector<vector<int>> nuevoEstado = copiarEstado(actual->estado);
                        swap(nuevoEstado[i][j], nuevoEstado[ni][nj]);
                        
                        string estadoStr = estadoAString(nuevoEstado);
                        if (visitados.find(estadoStr) == visitados.end()) {
                            visitados.insert(estadoStr);
                            int h_nuevo = calcularHeuristica(nuevoEstado);
                            Nodo* sucesor = new Nodo(nuevoEstado, actual->costo_g + 1, h_nuevo, actual);
                            frontera.push(sucesor);
                        }
                    }
                }
            }
            return {};
        }
};

//-----------CLASE DE CONTROL DEL JUEGO-----------
class Puzzle{
    private:
        vector<vector<int>> estadoInicial;
        vector<vector<int>> estadoFinal;
        
        void mostrarTablero(const vector<vector<int>>& tablero) {
            cout << "\n-------------\n";
            for (int i = 0; i < 3; i++) {
                cout << "| ";
                for (int j = 0; j < 3; j++) {
                    if (tablero[i][j] == 0) cout << "_ ";
                    else cout << tablero[i][j] << " ";
                    cout << "| ";
                }
                cout << "\n-------------\n";
            }
        }
        
        bool esSolucion(const vector<vector<int>>& tablero) {
            return tablero == estadoFinal;
        }
        
        vector<int> obtenerMovimientosValidos(const vector<vector<int>>& tablero) {
            vector<int> movimientos;
            pair<int, int> posCero;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (tablero[i][j] == 0) {
                        posCero = {i, j};
                        break;
                    }
                }
            }
            
            int i = posCero.first, j = posCero.second;
            if (i > 0) movimientos.push_back(tablero[i-1][j]);
            if (i < 2) movimientos.push_back(tablero[i+1][j]);
            if (j > 0) movimientos.push_back(tablero[i][j-1]);
            if (j < 2) movimientos.push_back(tablero[i][j+1]);
            
            return movimientos;
        }
        
        bool moverPieza(vector<vector<int>>& tablero, int numero) {
            pair<int, int> posCero, posNumero;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (tablero[i][j] == 0) posCero = {i, j};
                    if (tablero[i][j] == numero) posNumero = {i, j};
                }
            }
            
            int di = abs(posCero.first - posNumero.first);
            int dj = abs(posCero.second - posNumero.second);
            
            if ((di == 1 && dj == 0) || (di == 0 && dj == 1)) {
                swap(tablero[posCero.first][posCero.second], tablero[posNumero.first][posNumero.second]);
                return true;
            }
            return false;
        }
        
        int sugerirMovimiento(const vector<vector<int>>& tablero) {
            vector<int> movimientos = obtenerMovimientosValidos(tablero);
            if (!movimientos.empty()) {
                return movimientos[0];
            }
            return -1;
        }
        
    public:
        Puzzle(){
            estadoInicial.resize(3, vector<int>(3,0));
            estadoFinal.resize(3, vector<int>(3,0));
        }

        void mostrarMenuPrincipal() {
            double opc;
            int opcion;
            Clase_Usuario us;
            do{
                cout << "\n        PUZZLE 8         \n\n";
                cout << "\n------MENU PRINCIPAL-----:";
                cout << "\n0- Salir";
                cout << "\n1- Modo inteligente";
                cout << "\n2- Modo usuario"; 
                cout << "\n3- Explicaion del juego"; 
                cout << "\n4- Ver puntajes de los jugadores";
                cout << "\nElige una opcion: ";
                cin >> opc;

                if (cin.fail()){
			        cin.clear();
			        cin.ignore(1000,'\n');
                    opcion=500;
                }
		        else if (fmod(opc,1)!=0) opcion=500;
		        else opcion=static_cast<int>(opc);

                system("cls");

                switch (opcion){
                    case 1: menuModoInteligente(); break;
                    case 2: menuModoUsuario(); break;
                    case 3: explicarJuego(); break;
                    case 4: us.mostrarPuntajes(); break;
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                if(opcion == 2 || opcion == 0) limpiarMatrices();
                system("cls");
            }while(opcion!=0);
        }

    private:
        void menuModoInteligente() {            
            double opc;
            int opcion;

            do{
                cout << "\n        PUZZLE 8         \n\n";
                cout << "\n------MODO INTELIGENTE-----:";
                cout << "\n0- Salir";
                cout << "\n1- Ver especificaciones del modo inteligente";
                cout << "\n2- Iniciar juego";  
                cout << "\nElige una opcion: ";
                cin >> opc;

                if (cin.fail()){
			        cin.clear();
			        cin.ignore(1000,'\n');
                    opcion=500;
                }
		        else if (fmod(opc,1)!=0) opcion=500;
		        else opcion=static_cast<int>(opc);

                system("cls");

                switch (opcion){
                    case 1: verEspecificaciones('i'); break;
                    case 2: jugarModoInteligente(); break;
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                if(opcion == 2 || opcion == 0) limpiarMatrices();
                system("cls");
            }while(opcion!=0);                        
        }

        void jugarModoInteligente() {
            capturarInicioFin();
            vector<vector<int>> tableroInteligente = estadoInicial;

            A_star a(estadoFinal);
            vector<vector<vector<int>>> solucion = a.resolver(estadoInicial);

            if (solucion.empty()) {
                cout << "No se encontro solucion para el estado inicial proporcionado.\n";
                system("pause");
                return;
            }

            cout << "Solucion encontrada! Mostrando pasos...\n";
            for (size_t paso = 0; paso < solucion.size(); paso++) {
                system("cls");
                cout << "Paso " << paso + 1 << " de " << solucion.size() << ":\n";
                mostrarTablero(solucion[paso]);
                if (paso < solucion.size() - 1) {
                    Sleep(tiempo_espera);
                }
            }
            cout << "\nPuzzle resuelto!\n";
            system("pause");
        }

        void capturarInicioFin() {
            bool pantallaLimpia = false;
            bool matrizValida = false;

            cout << "\n        PUZZLE 8         \n\n";
            cout << "\n------MODO INTELIGENTE-----";
            cout << "\nIngresa los numeros para el estado inicial y final (solucion) del juego";
            cout << "\nDeben ser numeros entre 0 y 8 y ninguno debe repetirse\n";

            do{
                if (pantallaLimpia) {
                    cout << "\n        PUZZLE 8         \n\n";
                    cout << "\n------MODO INTELIGENTE-----";
                    cout << "\nIngresa los numeros para el estado inicial y final (solucion) del juego";
                    cout << "\nDeben ser numeros entre 0 y 8 y ninguno debe repetirse\n";
                }

                cout << "\nEstado inicial del puzzle\n";
                for(int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        cin >> estadoInicial[i][j];
                    }
                }

                if (!verificarEspacios(estadoInicial)) {
                    cout << "Intenta de nuevo\n";
                    system("pause");
                    system("cls");
                    pantallaLimpia = true;
                }
                else {
                    pantallaLimpia = false;
                    matrizValida = true;
                }
            }while(!matrizValida);

            matrizValida = false;
            do{
                if (pantallaLimpia) {
                    cout << "\n        PUZZLE 8         \n\n";
                    cout << "\n------MODO INTELIGENTE-----";
                    cout << "\nIngresa los numeros para el estado inicial y final (solucion) del juego";
                    cout << "\nDeben ser numeros entre 0 y 8 y ninguno debe repetirse\n";
                    cout << "\nEstado inicial del puzzle\n";
                    for(int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            cout << estadoInicial[i][j] << " ";
                        }
                        cout << endl;
                    }
                }

                cout << "\nEstado final (meta) del puzzle\n";
                for(int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        cin >> estadoFinal[i][j];
                    }
                }

                if (!verificarEspacios(estadoFinal)) {
                    cout << "Intenta de nuevo\n";
                    system("pause");
                    system("cls");
                    pantallaLimpia = true;
                }
                else {
                    pantallaLimpia = false;
                    matrizValida = true;
                }
            }while(!matrizValida);
        }

        bool verificarEspacios(vector<vector<int>> matriz) {
            vector<int> existentes;
            int revisados = 0;

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (matriz[i][j] < 0 || matriz[i][j] > 8) {
                        cout << "\nHay numeros fuera de rango: los numeros deben estar entre 0 y 8\n";
                        return false;
                    }
                    
                    if (revisados > 0) {
                        for (int k = 0; k < revisados; k++) {
                            if (matriz[i][j] == existentes[k]) {
                                cout << "\nHay numeros repetidos: "<< matriz[i][j] << "\n";
                                return false;
                            } 
                        }
                    }

                    existentes.push_back(matriz[i][j]);
                    revisados++;
                }
            }
            return true;
        }

        void menuModoUsuario(){
            float opc;
            int opcion;
            int nivel = 1;

            do{
                cout << "\n        PUZZLE 8         \n\n";
                cout << "\n------MODO USUARIO-----:";
                cout << "\n0- Salir";
                cout << "\n1- Ver especificaciones del modo usuario";
                cout << "\n2- Elegir nivel (nivel actual: " << nivel << ")";
                cout << "\n3- Iniciar juego"; 
                cout << "\nElige una opcion: ";
                cin >> opc;

                if (cin.fail()){
			        cin.clear();
			        cin.ignore(1000,'\n');
                    opcion=500;
                }
		        else if (fmod(opc,1)!=0) opcion=500;
		        else opcion=static_cast<int>(opc);

                system("cls");

                switch (opcion){
                    case 1: verEspecificaciones('u'); break;
                    case 2: nivel = cambiarNivel(nivel); break;
                    case 3: jugarModoUsuario(nivel); break;
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                if(opcion == 3) limpiarMatrices();
            }while(opcion!=0);            
        }

        void jugarModoUsuario(int nivel) {
            generarNuevoJuego(nivel);
            vector<vector<int>> tableroUsuario = estadoInicial;
            int movimientos = 0;
            int puntaje_acumulado = 0;
            bool salir = false;

            while (!esSolucion(tableroUsuario) && !salir) {
                system("cls");
                cout << "Movimientos: " << movimientos << " | Puntaje: " << puntaje_acumulado << endl;
                cout << "Nivel: " << nivel << endl;
                mostrarTablero(tableroUsuario);
                
                vector<int> movimientosValidos = obtenerMovimientosValidos(tableroUsuario);
                cout << "Movimientos validos: ";
                for (int num : movimientosValidos) cout << num << " ";
                cout << endl;
                
                cout << "Opciones:\n";
                cout << "1- Mover pieza\n";
                cout << "2- Sugerir movimiento\n";
                cout << "3- Salir\n";
                cout << "Elige una opcion: ";
                
                int opcion;
                cin >> opcion;
                
                switch (opcion) {
                    case 1: {
                        cout << "Ingresa el numero a mover: ";
                        int numero;
                        cin >> numero;
                        if (moverPieza(tableroUsuario, numero)) {
                            movimientos++;
                            puntaje_acumulado = totCorrectos(tableroUsuario);
                        } else {
                            cout << "Movimiento invalido!\n";
                            system("pause");
                        }
                        break;
                    }
                    case 2: {
                        int sugerencia = sugerirMovimiento(tableroUsuario);
                        if (sugerencia != -1) {
                            cout << "Sugerencia: mueve el numero " << sugerencia << endl;
                        } else {
                            cout << "No hay movimientos validos\n";
                        }
                        system("pause");
                        break;
                    }
                    case 3:
                        salir = true;
                        break;
                    default:
                        cout << "Opcion invalida\n";
                        system("pause");
                }
            }
            
            if (esSolucion(tableroUsuario)) {
                cout << "Felicidades! Has resuelto el puzzle!\n";
                cout << "Movimientos totales: " << movimientos << endl;
                puntaje_acumulado += (100 - movimientos);
            }
            
            if (!salir) {
                pedirDatosUsuario(puntaje_acumulado);
            }
            
            Clase_Usuario us;
            us.mostrarPuntajes();
            system("pause");
        }

        void generarNuevoJuego(int nivel) {
            // Estado final fijo
            int contador = 1;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    estadoFinal[i][j] = contador % 9;
                    contador++;
                }
            }

            // Generar estado inicial aleatorio
            vector<int> numeros = {0,1,2,3,4,5,6,7,8};
            random_shuffle(numeros.begin(), numeros.end());
            
            int index = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    estadoInicial[i][j] = numeros[index++];
                }
            }

            // Para el nivel 1, asegurar que sea solucionable y mas facil
            if (nivel == 1) {
                // Hacer algunos movimientos desde el estado final
                estadoInicial = estadoFinal;
                int movimientos = 5 + rand() % 6; // 5-10 movimientos
                for (int i = 0; i < movimientos; i++) {
                    vector<int> movimientosValidos = obtenerMovimientosValidos(estadoInicial);
                    if (!movimientosValidos.empty()) {
                        int movimiento = movimientosValidos[rand() % movimientosValidos.size()];
                        moverPieza(estadoInicial, movimiento);
                    }
                }
            }
        }

        int totCorrectos(vector<vector<int>> tableroUsuario) {
            int correctos = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (tableroUsuario[i][j] == estadoFinal[i][j]) 
                        correctos += 10;
                }
            }
            return correctos;
        }

        void pedirDatosUsuario(int puntaje_acumulado) {
            system("cls");
            char nombre[TAM];
            Clase_Usuario us; 
            
            cout << "\n-----FIN DEL JUEGO-----";
            cout << "\nLo hiciste muy bien! Cual es tu nombre?\n";
            cout << "Nombre: "; 
            cin.ignore();
            cin.getline(nombre, TAM);

            us.guardarNombrePuntaje(nombre, puntaje_acumulado);
        }
    
        int cambiarNivel(int nivel_actual) {
            return (nivel_actual == 1) ? 2 : 1;
        }

        void limpiarMatrices() {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    estadoInicial[i][j] = 0;
                    estadoFinal[i][j] = 0;
                }
            }            
        }

        void verEspecificaciones(char modo) {
            cout << "\n------ESPECIFICACIONES ";
            if (modo == 'u') {
                cout << "MODO USUARIO------\n";
                cout << "En este modo:\n";
                cout << "- Se genera un tablero aleatorio\n";
                cout << "- Debes ordenar los numeros del 1-8 con el 0 al final\n";
                cout << "- Puedes mover piezas adyacentes al espacio vacio\n";
                cout << "- Ganas puntos por cada pieza en posicion correcta\n";
                cout << "- Tienes sugerencias de movimientos\n";
            } else {
                cout << "MODO INTELIGENTE------\n";
                cout << "En este modo:\n";
                cout << "- Tu defines el estado inicial y final\n";
                cout << "- El programa resuelve el puzzle automaticamente\n";
                cout << "- Usa el algoritmo A* con heuristica Manhattan\n";
                cout << "- Muestra la solucion paso a paso\n";
            }
            system("pause");
        }

        void explicarJuego() {
            cout << "\n------PUZZLE 8-----\n";
            cout << "El Puzzle 8 es un juego de deslizamiento donde debes ordenar\n";
            cout << "los numeros del 1 al 8 en orden, dejando el espacio vacio (0)\n";
            cout << "al final. Solo puedes mover piezas adyacentes al espacio vacio.\n";
            cout << "El objetivo es lograr la configuracion final en el menor numero\n";
            cout << "de movimientos posible.\n";
            system("pause");
        }
};

int main() {
    srand(time(NULL));
    Puzzle p;
    p.mostrarMenuPrincipal();
    system("cls");
    cout << endl;
    cout << " --------------------------\n";
    cout << "|         GRACIAS          |\n";
    cout << "|           POR            |\n";
    cout << "|          JUGAR           |\n"; 
    cout << " --------------------------";           
    return 0;
}