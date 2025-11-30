#include <iostream>
#include <math.h>       // Uso de fmod() para validar entradas 
#include <stdlib.h>     // Generación de alatorios
#include <ctime>        // Gneración de aleatorios y manejo obtención de tiempo actual
#include <cctype>   
#include <windows.h>    // Uso de Sleep(), system() y colores 
#include <conio.h>      // Captura de teclas de flecha 
#include <fstream>      // Manejo de archicos 
#include <string>       // Manejo de strings 
#include <vector> 
#include <queue> 
#include <set>
#include <map>
#include <algorithm>
#include <functional>  // Necesario para std::function

using namespace std;

#define TAM 500 // Tamño maximo para cadena de nombre

class Clase_Usuario { 
    private:
        // Datos del usuario para guardar en el archivo     
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
            Datos_Usuario nuevo = {}, existente = {};
            fstream puntajes;
            bool existe = false;
            streampos posicion;

            // Abrir archico para lectoescritura 
            puntajes.open("puntajes.dat", ios::binary | ios::in | ios::out);

            // Si el archivo no existe, crearlo 
            if(!puntajes) {
                puntajes.open("puntajes.dat", ios::binary | ios::out);
                puntajes.close();
                puntajes.open("puntajes.dat", ios::binary | ios::in | ios::out);
                
                // Verificar si se abrió el archivo correctamente 
                if(!puntajes) {
                    cerr << "\nNo se pudo abrir el archivo para guardar puntajes\n";
                    return;
                }
            }

            // Buscar si el usuario ya existe (recorrer el archivo)
            while (puntajes.read(reinterpret_cast<char*>(&existente), sizeof(Datos_Usuario))) {
                // Comparar los nombres registrados con el nuevo nombre 
                if (strcmp(existente.nombreUsuario, nombre) == 0) {
                    existe = true;
                    // Posición del registro a modificar = inicio de registro 
                    // La lectura te deja al inicio del siguiente registro, 
                    // puntajes.tellg() es esa posición menos el tamaño del registro = posición del regisrtro que se desea modificar  
                    posicion = puntajes.tellg() - streampos(sizeof(Datos_Usuario));
                    break;
                }
            }

            // Si el usuario ya existía 
            if (existe) {
                // Sumar puntuación anteiror más la nueva
                existente.puntuacion += puntaje;
                // Actualizar la fecha a la fecha actual 
                time_t hoy = time(0);
                tm * timeinfo = localtime(&hoy);
                existente.dia.dd = timeinfo->tm_mday; 
                existente.dia.mm = timeinfo->tm_mon + 1; 
                existente.dia.aa = 1900 + timeinfo->tm_year; 
                
                // Escribir los datos actulizados en el registro 
                puntajes.seekp(posicion);
                puntajes.write(reinterpret_cast<char*>(&existente), sizeof(Datos_Usuario));
            } 
            // Si es un nuevo usuario grabar los daros de manera normal 
            else {
                // Inicializar al nuevo
                memset(&nuevo, 0, sizeof(Datos_Usuario)); // Limpiar estructura
                // Guardar los datos en nuevo
                strcpy(nuevo.nombreUsuario, nombre);
                nuevo.puntuacion = puntaje;
                time_t hoy = time(0);
                tm * timeinfo = localtime(&hoy);
                nuevo.dia.dd = timeinfo->tm_mday;
                nuevo.dia.mm = timeinfo->tm_mon + 1;
                nuevo.dia.aa = 1900 + timeinfo->tm_year;
                
                puntajes.clear(); // Limpiar posición del registro 
                puntajes.seekp(0, ios::end);  // Posicionarse al final del registro 
                puntajes.write(reinterpret_cast<char*>(&nuevo), sizeof(Datos_Usuario)); // Escribir los datos en el registro 
            }

            puntajes.close(); // Cerrar archivo 
        }

        void mostrarPuntajes() {
            Datos_Usuario guardado = {};
            ifstream puntajes;

            // Abrir archivo para lectura 
            puntajes.open("puntajes.dat", ios::binary | ios::in);

            // Verificar si el archivo existe 
            if (!puntajes) {
                cout << "\nNo hay puntajes guardados aún.\n";
                system("pause");
                return;
            }

            // Verificar si el archivo está vaçío 
            puntajes.seekg(0, ios::end); // Posicionarse al final del archivo 
            if (puntajes.tellg() == 0) { // Si el tamaño al fianl del archivo es 0, entonces está vacío
                cout << "\nNo hay puntajes guardados aún.\n";
                puntajes.close();
                return;
            }
            puntajes.seekg(0, ios::beg); // Si no estaba vacío, volver al inicio del archivo 

            cout << "\n                PUZZLE 8         \n\n";
            cout << "------------------PUNTAJES------------------\n";
            cout << "Nombre\t\tPuntaje\tFecha\n";
            cout << "--------------------------------------------\n";

            // Mostrar todos los puntajes guardados 
            while (puntajes.read(reinterpret_cast<char*>(&guardado), sizeof(Datos_Usuario))) {
                if(guardado.nombreUsuario[0] != '\0') {
                cout << guardado.nombreUsuario << "\t\t" 
                     << guardado.puntuacion << "\t"  
                     << guardado.dia.dd << "/" << guardado.dia.mm << "/" << guardado.dia.aa << endl;
                    }
                }
            puntajes.close(); // Cerrar archivo 
            cout << "\n--------------------------------------------\n";

            system("pause");
        }
};

//-----------CLASE PARA RESOLVER CON A*-----------
class A_star {
    private:
        struct Nodo {
            vector<vector<int>> estado; // Estado actual del tablero: posición de las piezas 
            int costo_g; // COSTO REAL para llegar desde el estado actual hasta el estado final 
            int costo_h; // HUERÍSTICA:  estimación del costo para llegar del estado actual al estado final 
            int costo_f; // Costo total del estimado de la solución .f(n) = g(n) + h(n), n es el estado actual 
            Nodo* padre; // Nodo anteriro, para reconstruir el camino 
            
            Nodo(vector<vector<int>> e, int g, int h, Nodo* p = nullptr) 
                : estado(e), costo_g(g), costo_h(h), padre(p) {
                costo_f = costo_g + costo_h; // Calculo del costo total
            }
        };
        
        // Función de comparación para la priority_queue
        struct CompararNodo {
            bool operator()(const Nodo* a, const Nodo* b) const {
                return a->costo_f > b->costo_f; // Para MIN_HEAP: menor costo primero 
            }
        };
        
        vector<vector<int>> estadoFinal; // Vector de estado final (meta del puzzle), aquí se copia el estado final 
        
        // Calcular huerítica con distancia de Manhattan 
        int calcularHeuristica(const vector<vector<int>>& estado) {
            int distancia = 0; // Variable para almacenar la distancia de Manhattan

            // Los dos primeros ciclos son para recorrer todas las posiciones del tablero actual 
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int valor = estado[i][j]; // Valor en la posición actual del tablero a revisar 
                    if (valor != 0) { // Si el valor no es el estado vacío(estado vacío no contribuye a la distancia)

                        // Recorrer estadoFinal para encontrar la posición objetivo de este valor
                        for (int k = 0; k < 3; k++) {
                            for (int l = 0; l < 3; l++) {
                                if (estadoFinal[k][l] == valor) {
                                    // Distancia (h) = sumatoria de las distancias dde todas las piezas (|r - r\*| + |c - c\*|). 
                                    // r,c = posición actual y r\*,c\* = posición objetivo 
                                    // r = i, c = j, r\* = k, c\* = l
                                    distancia += abs(i - k) + abs(j - l);
                                    break; // Terminar si ya se calculó la distancai para una pieza (para buscar la siguiente)
                                }
                            }
                        }

                    }
                }
            }

            // Regresar la distancia obtenida 
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
        A_star(vector<vector<int>> final) : estadoFinal(final) {} // Copiar estado final 
        
        // Algorimo de A* (A star), devuelve un vector con las matrices del paso paso 
        vector<vector<vector<int>>> resolver(vector<vector<int>> estadoInicial) {
            // Cola de proirdad a la frontera de búsqueda 
            priority_queue<Nodo*, vector<Nodo*>, CompararNodo> frontera;
            // Set para evitar repeticiones 
            set<string> visitados;
            
            // Calcular huerísitca inicial usando Distancia de Manhattan 
            int h_inicial = calcularHeuristica(estadoInicial);

            // Crear nodo inicial 
            Nodo* nodoInicial = new Nodo(estadoInicial, 0, h_inicial);

            // Agregar nodo inicail a la cola de prioridad 
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

            // Retornar matriz vacía si no se encontró solución 
            return {};
        }
};

//-----------CLASE DE CONTROL DEL JUEGO-----------
class Puzzle{
    private:
        // Matrices para estado final e inicial, se usan en los dos modos de juego 
        vector<vector<int>> estadoInicial;
        vector<vector<int>> estadoFinal;
        
        // Mostrar tablero para modo usuario 
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

        // Mostrar tablero para modo inteligente 
        void mostrarTablero(const vector<vector<int>>& tablero, const vector<vector<int>>& meta) {
            int f = 0; // Fila de la meta 
            cout << "\n-------------\t\t-------------\n";
            for (int i = 0; i < 3; i++) {
                cout << "| ";
                for (int j = 0; j < 3; j++) {
                    if (tablero[i][j] == 0) cout << "_ ";
                    else cout << tablero[i][j] << " ";
                    cout << "| ";
                    if (j == 2) {
                        cout << "\t\t| ";
                        for (int k = 0; k < 3; k++) {
                            if (meta[f][k] == 0) cout << "_ ";
                            else cout << meta[f][k] << " ";
                            cout << "| ";
                        }
                        f++;
                    }
                }
                cout << "\n-------------\t\t-------------\n";
            }
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
                
    public: 
        // Crear e inicizalizar matrices de estado inicial y final 
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
        //-----------FUNCIONES PARA MODO INTELIGENTE-----------
        void menuModoInteligente() {            
            double opc;
            int opcion;

            do{
                cout << "\n        PUZZLE 8         \n";
                cout << "\n------MODO INTELIGENTE-----\n";
                cout << "\n0- Salir";
                cout << "\n1- Ver especificaciones del modo inteligente";
                cout << "\n2- Iniciar juego";  
                cout << "\nElige una opcion: ";
                cin >> opc;

                // Verificar entrada válida 
                if (cin.fail()){ // Si la entrada no es un número
			        cin.clear(); // Limpiar estado de error de cin 
			        cin.ignore(1000,'\n'); // Descartar la entrada incorrecta hasta mil caracteres o hasta encontrar un salto de línea
                    opcion=500;
                }
		        else if (fmod(opc,1)!=0) opcion=500; // Descartar números con decimales 
		        else opcion=static_cast<int>(opc);  // Convertir entrada a entero si es válida 

                system("cls");

                switch (opcion){
                    case 1: explicarJuego('i'); break;
                    case 2: jugarModoInteligente(); break;
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                // Limpiar matrices de estado inicial y final antes de un juego nuevo  
                if(opcion == 2 || opcion == 0) limpiarMatrices();
                system("cls");
            }while(opcion!=0);                        
        }

        void jugarModoInteligente() {
            // Capturar estado de inicial y final (meta) del puzzle
            capturarInicioFin();  

            // Copiar el tablero incial ingresado por el usuario para el estado inical del modo inteligente 
            vector<vector<int>> tableroInteligente = estadoInicial;

            // Buscar si existe una solución para los estados incial y final ingresados por el usuario
            cout << "Buscando si existe solucion...\n";
            A_star a(estadoFinal);
            // Vector que guarda las matrices con el paso a paso para llegar a la solución 
            vector<vector<vector<int>>> solucion = a.resolver(estadoInicial);

            if (solucion.empty()) {
                cout << "No se encontro solucion para el estado inicial proporcionado.\n";
                system("pause");
                return;
            }

            // Pedir el tiempo de espera para mostrar el paso a paso en segundos 
            int tiempo_espera = pedirTiempoEspera();

            // Mostrar solución encontrada 
            bool salir = false;
            for (size_t paso = 0; paso < solucion.size(); paso++) {
                // Capturar tecla  
                if (_kbhit()) {
                    char tecla = _getch();

                    // Salir: dejar de ver la solución 
                    if (tecla == '1') {
                        salir = true;
                        break;
                    }

                    // Ver espicificaiones del modo usuario 
                    else if (tecla == '2') {
                        system("cls");
                        explicarJuego('i');
                    }

                    // Cambiar la velocidad 
                    else if (tecla == -32 || tecla == 0) tiempo_espera = cambiarTiempoEspera(tiempo_espera); 
                }

                system("cls");
                cout << "\n        PUZZLE 8         \n";
                cout << "\n------MODO INTELIGENTE-----\n";
                cout << "Salir: 1 \t\tVer especificaiones: 2" << endl;
                cout << "Velocidad: " << tiempo_espera/1000 << " segundos" << endl;
                cout << "Paso " << paso + 1 << " de " << solucion.size() << "\t\t";
                cout << "Meta a alcanzar\n";
                mostrarTablero(solucion[paso], estadoFinal);
                if (paso < solucion.size() - 1) {
                    Sleep(tiempo_espera);
                }
            }
            if (!salir) {
                cout << "\nPuzzle resuelto!\n";
                system("pause");
            }
        }

        // Capturar estado inicial y final (meta) del puzzle, el usuario lo ingresa 
        void capturarInicioFin() {
            bool pantallaLimpia = false; // Limpiar pantalla en caso de una entrada inválida 
            bool matrizValida = false;   // Condición de finalización de los ciclos do-while (termian si la entrada para la matriz es válida)

            cout << "\n        PUZZLE 8         \n";
            cout << "\n------MODO INTELIGENTE-----";
            cout << "\nIngresa los numeros para el estado inicial y final (solucion) del juego";
            cout << "\nDeben ser numeros entre 0 y 8 y ninguno debe repetirse\n";

            // Capturar el estado inicial del puzzle 
            do{
                if (pantallaLimpia) {
                    cout << "\n        PUZZLE 8         \n";
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

                // ¿Entrada para estado inicial válida?
                if (!verificarEspacios(estadoInicial)) { // ¿Entrada para estado inicial válida?
                    cout << "Intenta de nuevo\n";
                    system("pause");
                    system("cls");
                    pantallaLimpia = true;
                }

                // Si la entrada fue válida
                else {
                    system("cls");
                    pantallaLimpia = true;
                    matrizValida = true;
                }
            }while(!matrizValida);

            matrizValida = false;
            do{
                if (pantallaLimpia) {
                    cout << "\n        PUZZLE 8         \n";
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

                // ¿Entrada para estado final (meta) válida?
                if (!verificarEspacios(estadoFinal)) { 
                    cout << "Intenta de nuevo\n";
                    system("pause");
                    system("cls");
                    pantallaLimpia = true;
                }

                // La matriz inicial y final no pueden ser iguales porque entonces no hay nada para resolver
                else if (estadoFinal == estadoInicial) {  
                    cout << "El estado final no puede ser igual al estado incial\n";
                    cout << "Intenta de nuevo\n";
                    system("pause");
                    system("cls");
                    pantallaLimpia = true;
                }

                // Si la entrada fue válida 
                else {
                    pantallaLimpia = false;
                    matrizValida = true;
                    system("cls");
                }
            }while(!matrizValida);
        }

        int pedirTiempoEspera() {
            int tiempo_espera = 0;
            double tiempo = 0;
            bool tiempoValido = false;

            do {
                system("cls");
                cout << "\n        PUZZLE 8         \n";
                cout << "\n------MODO INTELIGENTE-----\n";
                cout << "Velocidad de la simulacion en segundos (entero, mayor a 0): ";
                cin >> tiempo;
                // Verificar entrada válida 
                if (cin.fail()) { // Si la entrada no es un número
			        cin.clear(); // Limpiar estado de error de cin 
			        cin.ignore(1000,'\n'); // Descartar la entrada incorrecta hasta mil caracteres o hasta encontrar un salto de línea
                    tiempo = 0;
                }
		        else if (fmod(tiempo,1) != 0) tiempo = 0 ; // Descartar números con decimales 
		        else if (tiempo > 0) {
                    tiempo_espera=static_cast<int>(tiempo);  // Convertir entrada a entero si es válida 
                    tiempo_espera *= 1000;
                    tiempoValido = true;
                }

            }while(!tiempoValido);

            return tiempo_espera;
        }

        int cambiarTiempoEspera(int tiempoActual) {
            char tecla = _getch();
                
            // Flecha arriba aumenta velocidad (disminuye tiempo)
            if (tecla == 72 && tiempoActual >= 1000) return (tiempoActual -  1000);

            // Flecha abajo disminuye velocidad (aumenta tiempo)
            if (tecla == 80) return (tiempoActual + 1000);

            // Devolver el tiempo original si se presiona otra tecla 
            return tiempoActual;
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

        //-----------FUNCIONES PARA MODO USUARIO-----------
        void menuModoUsuario(){
            float opc;
            int opcion;
            int nivel = 1; // El nivel por default es 1

            do{
                cout << "\n        PUZZLE 8         \n\n";
                cout << "\n------MODO USUARIO-----:";
                cout << "\n0- Salir";
                cout << "\n1- Ver especificaciones del modo usuario";
                cout << "\n2- Elegir nivel (nivel actual: " << nivel << ")";
                cout << "\n3- Iniciar juego"; 
                cout << "\nElige una opcion: ";
                cin >> opc;

                // Verificar entrada válida 
                if (cin.fail()) { // Si la entrada no es un número 
			        cin.clear(); // Limpiar estado de error de cin 
			        cin.ignore(1000,'\n'); // Descartar entrada incorrecta hasta mil caracteres o hasta salto de línea
                    opcion = 500;
                }
		        else if (fmod(opc,1)!=0) opcion=500; // Descaratar números con decimales 
		        else opcion=static_cast<int>(opc); // Convertir entrada a entero si es válida 

                system("cls");

                switch (opcion){
                    case 1: explicarJuego('u'); break;
                    case 2: nivel = cambiarNivel(nivel); break;
                    case 3: jugarModoUsuario(nivel); break;
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                // Limpiar matrices de estado antes de un nuevo juego
                if(opcion == 3) limpiarMatrices();
            }while(opcion!=0);            
        }

        void jugarModoUsuario(int nivel) {
            generarNuevoJuego(nivel); // Generar tableros para nuevo juego 
            vector<vector<int>> tableroUsuario = estadoInicial; // El tablero del usuario empiza en el estado inicial
            int movimientos = 0; // Total de movimientos realizados por el usuario para llegar al estado final 
            int puntaje_acumulado = 0;
            bool salir = false;

            // El usuario juega hasta que encuentra la solución o hasta que decide salir 
            while (!esSolucion(tableroUsuario) && !salir) { 
                system("cls");

                // El total de puntos es igual al total de piezas en su posición correcta * 10 
                // Se asigna puntaje a partir del primer movimiento 
                if (movimientos !=0) puntaje_acumulado = totCorrectos(tableroUsuario);

                cout << "\n        PUZZLE 8         \n";
                cout << "\n------MODO USUARIO-----\n";
                cout << "Movimientos: " << movimientos << " | Puntaje: " << puntaje_acumulado << endl;
                cout << "Nivel: " << nivel << endl;
                int opcion = 0;
                float opc = 0;
                
                mostrarTablero(tableroUsuario);
                
                // Vector de piezas que pueden ser movidas (son las adyacentes al espacio vacío)
                vector<int> movimientosValidos = obtenerMovimientosValidos(tableroUsuario);
                cout << "Movimientos validos: ";
                for (int num : movimientosValidos) cout << num << " ";
                cout << endl;
                
                cout << "Opciones:\n";
                cout << "(0-8)- Numero de la pieza a mover\n";
                cout << "9- Sugerir movimiento\n";
                cout << "10- Salir\n";
                cout << "11- Ver especificaciones\n";
                cout << "Elige una opcion: ";
                
                cin >> opc;

                // Verificar entrada válida 
                if (cin.fail()) { // Si la entrada no es un número 
			        cin.clear(); // Limpiar estado de error de cin 
			        cin.ignore(1000,'\n'); // Descartar entrada incorrecta hasta mil caracteres o hasta salto de línea
                    opcion=500;
                }
		        else if (fmod(opc,1)!=0) opcion=500; // Descaratar números con decimales 
		        else opcion=static_cast<int>(opc); // Convertir entrada a entero si es válida 
                
                if(opcion >= 0 && opcion <=8) {
                    if (moverPieza(tableroUsuario, opcion)) {
                        movimientos++;
                        puntaje_acumulado = totCorrectos(tableroUsuario);
                    } 
                    else {
                        cout << "Movimiento invalido!\n";
                        system("pause");
                    }
                }
                else if (opcion == 9) {
                    int sugerencia = sugerirMovimiento(tableroUsuario);
                    if (sugerencia != -1) {
                        cout << "Sugerencia: mueve el numero " << sugerencia << endl;
                    } 
                    else cout << "No hay movimientos validos\n";
                    system("pause");
                }
                else if (opcion == 10) salir = true;
                else if (opcion == 11) {
                    system("cls");
                    explicarJuego('u');
                }
                else {
                    cout << "Opcion invalida\n";
                    system("pause");
                }
            }
            
            system("cls");
            cout << "\n        PUZZLE 8         \n";
            cout << "\n------MODO USUARIO-----\n";
            mostrarTablero(tableroUsuario);
            cout << endl;

            if (esSolucion(tableroUsuario)) {
                cout << "Felicidades! Has resuelto el puzzle!\n";
                cout << "Movimientos totales: " << movimientos << endl;
            }
            
            // Al usuario se le piden sus datos si termina el juego: salió o completó el puzzle 
            pedirDatosUsuario(puntaje_acumulado);
            
            Clase_Usuario us;
            us.mostrarPuntajes();
            system("cls");
        }

        void generarNuevoJuego(int nivel) {
            // Estado final fijo: números del 0-8 
            int contador = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (i == 0 && j == 0) estadoFinal[i][j] = contador;
                    else estadoFinal[i][j] = contador % 9;
                    contador++;
                }
            }
            
            // Generar estado inicial aleatorio
            vector<int> numeros = {0,1,2,3,4,5,6,7,8};
            random_shuffle(numeros.begin(), numeros.end());
            
            int indice = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    estadoInicial[i][j] = numeros[indice++];
                }
            }

            // Para el nivel 1, asegurar que sea solucionable y más facil
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

        // Comparar estado actual del tablero con la meta 
        bool esSolucion(const vector<vector<int>>& tablero) {
            return tablero == estadoFinal;
        }

        // Función para obtener las piezas que son adyacentes al espaco vacío
        vector<int> obtenerMovimientosValidos(const vector<vector<int>>& tablero) {
            vector<int> movimientos;
            pair<int, int> posCero;

            // Ubicar la posición del estado vació (0) en el tablero 
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (tablero[i][j] == 0) {
                        posCero = {i, j};
                        break;
                    }
                }
            }
            
            // Agregar al vector de movimientos las piezas adyacentes al cero 
            int i = posCero.first, j = posCero.second;
            if (i > 0) movimientos.push_back(tablero[i-1][j]);
            if (i < 2) movimientos.push_back(tablero[i+1][j]);
            if (j > 0) movimientos.push_back(tablero[i][j-1]);
            if (j < 2) movimientos.push_back(tablero[i][j+1]);
            
            return movimientos; // Regresar los movimientos válidos encontrados 
        }

        int totCorrectos(vector<vector<int>> tableroUsuario) {
            int correctos = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (tableroUsuario[i][j] == estadoFinal[i][j]) correctos += 10;
                }
            }
            return correctos;
        }

        void pedirDatosUsuario(int puntaje_acumulado) {
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
            if (nivel_actual == 1) return 2; // Si el nivel es 1, cambiar a 2 
            return 1;                        // Si el nivel es 2, cambiar a 1
        }

        int sugerirMovimiento(const vector<vector<int>>& tablero) {

            vector<int> movimientosValidos = obtenerMovimientosValidos(tablero);
            if (movimientosValidos.empty()) return -1;
    
            int piezasCorrectas = contarPiezasCorrectas(tablero);
    
            if (piezasCorrectas <= 3) {
                // Priorizar primera fila y primera columna
                for (int num : movimientosValidos) {
                    if ((tablero[0][0] == num || tablero[0][1] == num || tablero[0][2] == num ||
                        tablero[1][0] == num || tablero[2][0] == num) &&
                        calcularDistanciaManhattan(num, tablero) > 0) {
                        return num;
                    }
                }
            }
            
            return movimientosValidos[0]; 
        }

        //---------Funciones auxiliares para sugerir movimiento----------
        int contarPiezasCorrectas(vector<vector<int>> tablero) {
            int contador = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (tablero[i][j] == estadoFinal[i][j]) {
                        contador++;
                    }
                }
            }
            return contador;
        }

        int calcularDistanciaManhattan (int num, vector<vector<int>> tablero) {
            pair<int,int> posActual, posObjetivo;

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (tablero[i][j] == num) {
                        posActual = {i,j};
                    }
                    if (estadoFinal[i][j] == num) {
                        posObjetivo = {i,j};
                    }
                }
            }

            // Distancia de Manhattan
            return abs(posActual.first - posObjetivo.first) + abs(posActual.second - posObjetivo.second);
        }

        //-----------OTRAS FUNCIONES-----------

        void limpiarMatrices() {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    estadoInicial[i][j] = 0;
                    estadoFinal[i][j] = 0;
                }
            }            
        }

        // Ver especificaciones del modo usauriio y del modo inteligente 
        void explicarJuego(char modo) {
            cout << "\n------ESPECIFICACIONES ";
            if (modo == 'u') {
                cout << "MODO USUARIO------\n";
                cout << "En este modo:\n";
                cout << "- Se genera un tablero aleatorio\n";
                cout << "- Debes ordenar los numeros del 1-8 hasta obtener un tablero como el siguiente:\n";
                int piezas = 0;
                cout << "\t\t\t-------------\n";
                for (int i = 0; i < 3; i++) {
                    cout << "\t\t\t";
                    cout << "| ";
                    for (int j = 0; j < 3; j++) {
                        if (piezas == 0) cout << "_ ";
                        else cout << piezas << " ";
                        cout << "| ";
                        piezas++;
                    }
                    cout << "\n\t\t\t-------------\n";
                }
                cout << endl;
                cout << "- Puedes mover piezas adyacentes al espacio vacio '_'\n";
                cout << "- Ganas puntos por cada pieza en posicion correcta\n";
                cout << "- Tienes sugerencias de movimientos\n";
                cout << "- Tu puntaje va a depender del total de piezas que estan en su lugar (1 pieza correcta = 10 puntos)\n";
                cout << "  Si no has realizado ningun movimiento tu puntaje sera 0\n";
            } else {
                cout << "MODO INTELIGENTE------\n";
                cout << "En este modo:\n";
                cout << "- Tu defines el estado inicial y final (meta) del puzzle\n";
                cout << "- El programa resuelve el puzzle automaticamente\n";
                cout << "- Muestra la solucion paso a paso\n";
                cout << "- Puedes cambiar la velocidad de la simulacion:\n";
                cout << "       - Flecha arriba para acelarar\n";
                cout << "       - Flecha abajo para hacer mas lento\n";
            }
            system("pause");
            system("cls");
        }

        // Explicaicón general del juego 
        void explicarJuego() {
            cout << "\n------PUZZLE 8-----\n";
            cout << "El Puzzle 8 es un juego de deslizamiento donde se debe ordenar\n";
            cout << "los numeros del 1 al 8 en orden, dejando el espacio vacio '_'\n";
            cout << "al inicio. Solo se pueden mover piezas adyacentes al espacio vacio.\n";
            cout << "El objetivo es lograr la configuracion final en el menor numero\n";
            cout << "de movimientos posible.\n\n";
            cout << "En el modo usuario deberas llegar a la solucion clasica, pero en el\n";
            cout << "modo inteligente tu decides cual sera la solucion (meta)\n";
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