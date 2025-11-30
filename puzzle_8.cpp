/*
-----------PROYECTO FINAL: 8_PUZZLE-----------

    -----------INTEGRANTES-----------
- Valeria Martín de Santos      ID: 343508
- Danna Sofía Morales Esparza   ID: 550210

      -----------MATERIA-----------
- Materia: Estrictura de Datos II
- Profesor: Eduardo Serna Pérez 

*/

//-----------LIBRERÍAS-----------
#include <iostream>
#include <math.h>       // Uso de fmod() para validar entradas numéricas 
#include <stdlib.h>     // Generación de alatorios (rand() y srand())
#include <ctime>        // Manejo de tiempo para aleatorios y obtención de tiempo actual
#include <cctype>       // Funciones para manejo de caracteres  
#include <windows.h>    // Uso de Sleep(), system("cls"), systen("pause") 
#include <conio.h>      // Captura de teclas especiales 
#include <fstream>      // Manejo de archivos para guardar puntajes 
#include <string>       // Manejo de cadenas de texto
#include <vector>       // Estructura de datos vector para vecotes y matrices 
#include <queue>        // Colas de prioridad para algoritmo A* (A star)
#include <set>          // Conjunto para evitar datos repetidos en A*
#include <map>          // Mapa para estructuras de datos auxiliares 
#include <algorithm>    // Funciones de ordenamiento y manipulación de contenedores 
#include <functional>   // Necesario para std::function en priority_queue

using namespace std;

#define TAM 500 // Tamño máximo para cadena de nombre de usuario 

//-----------CLASE MANEJO DE PUNTAJES Y DATOS DE USUARIO-----------
class Clase_Usuario { 
    private:
        // Estructura de datos del usuario para guardar en el archivo     
        struct Datos_Usuario {
            char nombreUsuario[TAM];    // Nombre del jugador
            int puntuacion;             //Puntaje acumulado del jugador
            struct Fecha {
                int dd; // Dia 
                int mm; // Mes 
                int aa; // Año
            }; 
            Fecha dia;
        };

        void ordenar_por_puntaje() {
            fstream puntajes;
            Datos_Usuario actual;
            vector<Datos_Usuario> usuarios;

            //Abrir archivo para lectura 
            puntajes.open("puntajes.dat", ios::binary | ios::in);
            
            // Verificar si se pudo abrir el archivo 
            if (!puntajes) {
                cout << "\nNo se pudo abrir el archivo para guardar puntajes.\n";
                system("pause");
                system("cls");
                return;
            }

            // Extraer a los usuarios registrados y ponerlos en un vector 
            while (puntajes.read(reinterpret_cast<char*>(&actual), sizeof(Datos_Usuario))) {
                usuarios.push_back(actual);
            }

            puntajes.close(); // Cerrar después de lectura 
            
            // Acomodar por puntajes de mayor a menor 
            sort(usuarios.begin(), usuarios.end(), [](const Datos_Usuario& a, const Datos_Usuario& b) {
                return a.puntuacion > b.puntuacion;
            });

            // Abrir archivo para escritura 
            puntajes.open("puntajes.dat", ios::binary | ios::out | ios::trunc);

            // Verificar si se pudo abrir el archivo 
            if (!puntajes) {
                cout << "\nNo se pudo abrir el archivo para guardar puntajes.\n";
                system("pause");
                system("cls");
                return;
            }

            // Grabar en el archivo los datos acomodados 
            for (size_t i = 0; i < usuarios.size(); i++) {
                puntajes.write(reinterpret_cast<char*>(&usuarios[i]), sizeof(Datos_Usuario));
                
            }

            puntajes.close(); // Cerrar archivo 
        }

    public:
        Clase_Usuario(){}
        
        /*
        Guardar datos del usurio en el archivo binario o 
        actualizar datos de usuarios existentes (fecha y puntaje)
        @param nombre: nombre del usuario
        @param puntaje: puntaje a sumar para el usuario 
        */
        void guardarNombrePuntaje(char nombre[TAM], int puntaje) {
            Datos_Usuario nuevo = {}, existente = {}; // Inicializar estructuras en 0
            fstream puntajes;
            bool existe = false;
            streampos posicion; // Para guardar posición de un usuario existente

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

            // Buscar si el usuario ya existe en el archivo 
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
                // Actualizar la fecha a la fecha actual del sistema
                time_t hoy = time(0);
                tm * timeinfo = localtime(&hoy);
                existente.dia.dd = timeinfo->tm_mday; 
                existente.dia.mm = timeinfo->tm_mon + 1;  // tm_mon va de 0-11
                existente.dia.aa = 1900 + timeinfo->tm_year; // tm_yaer es años desde 1900
                
                // Posicionarse y escribir los datos actulizados en el registro 
                puntajes.seekp(posicion);
                puntajes.write(reinterpret_cast<char*>(&existente), sizeof(Datos_Usuario));
            } 

            // Si es un nuevo usuario grabar los datos de manera normal 
            else {
                // Inicializar al nuevo
                memset(&nuevo, 0, sizeof(Datos_Usuario)); // Limpiar estructura
                // Copiar los datos en nuevo
                strcpy(nuevo.nombreUsuario, nombre);
                nuevo.puntuacion = puntaje;
                // Obtener fecha actual 
                time_t hoy = time(0);
                tm * timeinfo = localtime(&hoy);
                nuevo.dia.dd = timeinfo->tm_mday;
                nuevo.dia.mm = timeinfo->tm_mon + 1;
                nuevo.dia.aa = 1900 + timeinfo->tm_year;
                
                puntajes.clear(); // Limpiar posición del registro 
                // Posicionarse al final y escribir nuevo registro 
                puntajes.seekp(0, ios::end);  
                puntajes.write(reinterpret_cast<char*>(&nuevo), sizeof(Datos_Usuario));
            }

            puntajes.close(); // Cerrar archivo 

            ordenar_por_puntaje();
        }

        // Muestra todos los datos guardados en el archivo 
        void mostrarPuntajes() {
            Datos_Usuario guardado = {}; // Inicializar estructura 
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
            puntajes.seekg(0, ios::beg); // Si no estaba vacío, volver al inicio del archivo  para lectura

            cout << "\n                PUZZLE 8         \n\n";
            cout << "------------------PUNTAJES------------------\n";
            cout << "Nombre\t\tPuntaje\tFecha\n";
            cout << "--------------------------------------------\n";

            // Leer y mostrar todos los registros  
            while (puntajes.read(reinterpret_cast<char*>(&guardado), sizeof(Datos_Usuario))) {
                // Solo mostrar registros con nombre válido (no vacío)
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

//-----------CLASE PARA RESOLVER CON A* Y HUERÍSTICA DE MANHATTAN-----------
class A_star {
    private:
        // Estructra Nodo: representa un estado del puzzle en el árbol de búsqueda 
        struct Nodo {
            vector<vector<int>> estado; // Estado actual del tablero: posición de las piezas 
            int costo_g; // COSTO REAL para llegar desde el estado actual hasta el estado final 
            int costo_h; // HUERÍSTICA:  estimación del costo para llegar del estado actual al estado final 
            int costo_f; // Costo total del estimado de la solución .f(n) = g(n) + h(n), n es el estado actual 
            Nodo* padre; // Nodo anteriro, para reconstruir el camino 
            
            // Constructor del nodo 
            Nodo(vector<vector<int>> e, int g, int h, Nodo* p = nullptr) 
                : estado(e), costo_g(g), costo_h(h), padre(p) {
                costo_f = costo_g + costo_h; // CÁlculo del costo total (Fórmula de A*)
            }
        };
        
        // Función de comparación para la priority_queue
        struct CompararNodo {
            bool operator()(const Nodo* a, const Nodo* b) const {
                return a->costo_f > b->costo_f; // Para MIN_HEAP: menor costo primero 
            }
        };
        
        vector<vector<int>> estadoFinal; // Vector de estado final (meta del puzzle), aquí se copia el estado final 
        
        /*
        Calcular huerítica con distancia de Manhattan
        @param estado: estado actual del tablero 
        @return distacia: suma de distancia de Manhattan de todas las piezas 
        */  
        int calcularHeuristica(const vector<vector<int>>& estado) {
            int distancia = 0; // Variable para almacenar la distancia de Manhattan

            // Recorrer todas las posciones del tablero  
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int valor = estado[i][j]; // Valor en la posición actual del tablero a revisar 
                    if (valor != 0) { // Si el valor no es el estado vacío(estado vacío no contribuye a la distancia)

                        // Buscar posición objetivo del valor en estadoFinal 
                        for (int k = 0; k < 3; k++) {
                            for (int l = 0; l < 3; l++) {
                                if (estadoFinal[k][l] == valor) {
                                    // Distancia (h) = [sumatoria de las distancias de todas las piezas (|r - r\*| + |c - c\*|)]. 
                                    // r,c = posición actual y r\*,c\* = posición objetivo 
                                    // r = i, c = j, r\* = k, c\* = l
                                    distancia += abs(i - k) + abs(j - l);
                                    break; // Salir al encontrar posición objetivo 
                                }
                            }
                        }

                    }
                }
            }

            // Regresar la distancia obtenida 
            return distancia;
        }
        
        /*
        Encontrar posición del estdo vacío 
        @param estado: estado del tablero actual 
        @return pair(fila, columna) del espacio vacío 
        */
        pair<int, int> encontrarCero(const vector<vector<int>>& estado) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (estado[i][j] == 0) return {i, j};
                }
            }
        }
        
        /*
        Hacer copia del estado actual del tablero 
        @param estado: estado a copiar 
        @return nueva matriz con los mismos valores del estado actual 
        */
        vector<vector<int>> copiarEstado(const vector<vector<int>>& estado) {
            vector<vector<int>> copia(3, vector<int>(3));
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    copia[i][j] = estado[i][j];
                }
            }
            return copia;
        }
        
        /*
        Convertir estado actual del tablero a string para usar en un conjunto de visitados 
        @param estado: estado del tablero a convertir 
        @return s: string con el estado 
        */
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
        
        /* 
        Algorimo de A* (A star) 
        @param  estadoInicial: estado inicial del puzzle desde el que se va a comnezar a resolver
        @return camino: vector con otodos los estados desde el inicio hasta el final 
        */
        vector<vector<vector<int>>> resolver(vector<vector<int>> estadoInicial) {
            // Cola de proirdad a la frontera de búsqueda (min_heap)
            priority_queue<Nodo*, vector<Nodo*>, CompararNodo> frontera;
            // Set para evitar repeticiones 
            set<string> visitados;
            
            // Calcular huerísitca inicial usando Distancia de Manhattan 
            int h_inicial = calcularHeuristica(estadoInicial);

            // Crear nodo inicial 
            Nodo* nodoInicial = new Nodo(estadoInicial, 0, h_inicial);

            // Agregar nodo inicial a la cola de prioridad 
            frontera.push(nodoInicial);
            visitados.insert(estadoAString(estadoInicial));
            
            // Ciclo principal para el algoritmo de A*
            while (!frontera.empty()) {
                // Obtener nodo con menor costo f
                Nodo* actual = frontera.top();
                frontera.pop();
                
                // Verificar si se encontró la solución
                if (actual->estado == estadoFinal) {
                    // Reconstruir camiono desde la ssolución hasta el inicio 
                    vector<vector<vector<int>>> camino; //Vector para gaurdar el paso a paso 
                    Nodo* temp = actual;
                    while (temp != nullptr) {
                        camino.push_back(temp->estado);
                        temp = temp->padre;
                    }
                    reverse(camino.begin(), camino.end()); // Invertir para tener de inicio a fin 
                    
                    // Limpiar memoria de todos los nodos en la fontera 
                    while (!frontera.empty()) {
                        delete frontera.top();
                        frontera.pop();
                    }
                    return camino;
                }
                
                // Encontrar posición del cero 
                auto posCero = encontrarCero(actual->estado);
                int i = posCero.first, j = posCero.second;
                
                // Movimientos posibles: arriba, abajo, izquierda, derecha 
                vector<pair<int, int>> movimientos = {{-1,0}, {1,0}, {0,-1}, {0,1}};
                
                // Generar todos los estados sucesores para la solución 
                for (auto mov : movimientos) {
                    int ni = i + mov.first, nj = j + mov.second;

                    // Verificar que el movimeinto esté dentro de los límites del tablero 
                    if (ni >= 0 && ni < 3 && nj >= 0 && nj < 3) {
                        // Generar nuevo estado moviendo el 0 con la pieza adyacente 
                        vector<vector<int>> nuevoEstado = copiarEstado(actual->estado);
                        swap(nuevoEstado[i][j], nuevoEstado[ni][nj]);
                        
                        // Verificar que el estado no haya sido visitado 
                        string estadoStr = estadoAString(nuevoEstado);
                        if (visitados.find(estadoStr) == visitados.end()) {
                            visitados.insert(estadoStr); // Marcar estado como visitado 
                            int h_nuevo = calcularHeuristica(nuevoEstado);

                            // Crear nuevo nodo sucesro y agregarlo a la frontera 
                            Nodo* sucesor = new Nodo(nuevoEstado, actual->costo_g + 1, h_nuevo, actual);
                            frontera.push(sucesor);
                        }
                    }
                }
            }

            // Retornar vector vacío si no se encontró solución 
            return {};
        }
};

//-----------CLASE DE CONTROL DEL JUEGO-----------
class Puzzle{
    private:
        // Matrices para estado final e inicial, se usan en los dos modos de juego 
        vector<vector<int>> estadoInicial;
        vector<vector<int>> estadoFinal;
                                                
    public: 
        // Constructor para crear e inicizalizar matrices de estado inicial y final 
        Puzzle(){
            estadoInicial.resize(3, vector<int>(3,0));
            estadoFinal.resize(3, vector<int>(3,0));
        }

        // Menú pricipal del juego 
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

                // Verificar entrada válida 
                if (cin.fail()){ // Si la entrada no es un número 
			        cin.clear(); // Limpiar estado de error de cin 
			        cin.ignore(1000,'\n'); // Descartar entrad incorrecta hasta mil caracteres o hasta encontrar un salto de línea
                    opcion = 500;
                }
		        else if (fmod(opc,1)!=0) opcion=500; // Descaratr números con decimales 
		        else opcion=static_cast<int>(opc); // Convertir entrada a entero si es válida 

                system("cls");

                switch (opcion){
                    case 1: menuModoInteligente(); break;
                    case 2: menuModoUsuario(); break;
                    case 3: explicarJuego(); break;
                    case 4: us.mostrarPuntajes(); break;
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                // Limpiar amtrices antes de un nuevo juego 
                if(opcion == 2 || opcion == 0) limpiarMatrices();
                system("cls");
            }while(opcion!=0);
        }

    private:
        //-----------FUNCIONES PARA MODO INTELIGENTE-----------
        // Menú de opciones para el modo inteligente 
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

        // Función para el control de la jugabilidad en modo inteligente 
        void jugarModoInteligente() {
            // Capturar estado de inicial y final (meta) del puzzle
            capturarInicioFin();  

            // Copiar el tablero incial ingresado por el usuario para el estado inical del modo inteligente 
            vector<vector<int>> tableroInteligente = estadoInicial;

            // Buscar si existe una solución con algoritmo A*
            cout << "Buscando si existe solucion...\n";
            A_star a(estadoFinal);
            // Vector que guarda las matrices con el paso a paso para llegar a la solución 
            vector<vector<vector<int>>> solucion = a.resolver(estadoInicial);

            //  Verificar si se encontró solución 
            if (solucion.empty()) {
                cout << "No se encontro solucion para el estado inicial proporcionado.\n";
                system("pause");
                return;
            }

            // Pedir el tiempo de espera para mostrar el paso a paso en segundos (el usuario decide la velocidad)
            int tiempo_espera = pedirTiempoEspera();

            // Mostrar solución encontrada paso a paso 
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

                    // Cambiar la velocidad con flechas 
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

            if (!salir) { // Mostrar mensaje de finalización si no se salió 
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
                // Re-mostrar instrucciones en caso de entrad inválida 
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

        /*
        Pedir al usuario el tiempo de espera netre pasos
        @return tiempo_espera: tiempo en milisegundos (mínimo un segundo)
        */
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
                    tiempo_espera *= 1000; // Coveritr a milisegundos (Sleep() usa milisegundos)
                    tiempoValido = true;
                }

            }while(!tiempoValido);

            return tiempo_espera; // Devolver tiempo original si se presiona otra tecla 
        }

        /*
        Cambiar tiempo de espera durante la simulación, con flechas
        @param tiempoActual: tiempo actual en milisegundos
        @return nuevo tiempo de espera 
        */
        int cambiarTiempoEspera(int tiempoActual) {
            char tecla = _getch();
                
            // Flecha arriba aumenta velocidad (disminuye tiempo), lo más rápido es un segundo 
            if (tecla == 72 && tiempoActual >= 1000) return (tiempoActual -  1000);

            // Flecha abajo disminuye velocidad (aumenta tiempo)
            if (tecla == 80) return (tiempoActual + 1000);

            // Devolver el tiempo original si se presiona otra tecla 
            return tiempoActual;
        }

        /*
        Mostrar tablero para modo inteligente
        @param tablero: estado actual del tablero 
        @param meta: estado final (meta) del tablero 
        */  
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

        /*
        Verificar que una matriz de espacios tenga números válidos (0-8) sin repeticiones 
        @param matriz: matriz a revisar 
        @return true si es válida, false si no lo es 
        */
        bool verificarEspacios(vector<vector<int>> matriz) {
            vector<int> existentes; // Vector para números ya vistos 
            int revisados = 0;      // Contador de números revisados 

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    // Verificar rango (0-8)
                    if (matriz[i][j] < 0 || matriz[i][j] > 8) {
                        cout << "\nHay numeros fuera de rango: los numeros deben estar entre 0 y 8\n";
                        return false;
                    }
                    
                    // Verificar duplicados a partir del segundo número
                    if (revisados > 0) {
                        for (int k = 0; k < revisados; k++) {
                            if (matriz[i][j] == existentes[k]) {
                                cout << "\nHay numeros repetidos: "<< matriz[i][j] << "\n";
                                return false;
                            } 
                        }
                    }

                    // Agregar números a la lista de existentes 
                    existentes.push_back(matriz[i][j]);
                    revisados++;
                }
            }
            return true; // Matriz válida   
        }

        //-----------FUNCIONES PARA MODO USUARIO-----------

        // Menú de opciones para el modo usuario 
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

        /*
        Función para el control del juego en modo inteligente 
        @param nivel: nivel de dificultad elegido por el usuario 
        */
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
                
                // Porcesar opción seleccionada 
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
            
            // Mostrar resultado final 
            system("cls");
            cout << "\n        PUZZLE 8         \n";
            cout << "\n------MODO USUARIO-----\n";
            mostrarTablero(tableroUsuario);
            cout << endl;

            if (esSolucion(tableroUsuario)) {
                cout << "Felicidades! Has resuelto el puzzle!\n";
                cout << "Movimientos totales: " << movimientos << endl;
            }
            
            // Guardar datos de usuario solo si hizo por lo menos un movimiento 
            if (movimientos > 0)pedirDatosUsuario(puntaje_acumulado);
            
            // Mostrar tabla de puntajes 
            Clase_Usuario us;
            us.mostrarPuntajes();
            system("cls");
        }

        /*
        Generar nuevo juego con estado inical aletorio 
        @param nivel: nivel de dificultad 
        */
        void generarNuevoJuego(int nivel) {
            // Estado final fijo: números del 0-8 en orden 
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

            // Para el nivel 1, asegurar que sea más facil
            if (nivel == 1) {
                // Partir del estado final y hacer algunos movimientos aleatorios
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

        /*
        Verificar si el estado actual del tablero es la solución 
        @param tablero: estado actual del tablero 
        @return true si es la solución, false si no 
        */ 
        bool esSolucion(const vector<vector<int>>& tablero) {
            return tablero == estadoFinal;
        }

        /*
        Mostrar tablero para modo usuario
        @param tablero: estado actual del tablero 
        */
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

        /*
        Mover pieza en el tablero si es un movimiento válido 
        @param tablero: estado actual del tablero 
        @param numero: pieza que se quiere mover 
        */
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

        /*
        Obtener piezas adyacentes al espacio vacío (son las que pueden moverse)
        @param tablero: estado actual del tablero 
        @return movimientos: vector con números de las piezas que pueden moverse 
        */
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
            
            // Verificar las cuatro direcciones posibles y 
            // agregar al vector de movimientos las piezas adyacentes al cero 
            int i = posCero.first, j = posCero.second;
            if (i > 0) movimientos.push_back(tablero[i-1][j]); // Arriba 
            if (i < 2) movimientos.push_back(tablero[i+1][j]); // Abajo
            if (j > 0) movimientos.push_back(tablero[i][j-1]); // Izquierda
            if (j < 2) movimientos.push_back(tablero[i][j+1]); // Derecha 
            
            return movimientos; // Regresar los movimientos válidos encontrados 
        }

        /*
        Calcular el total de piezas en su posición correcta 
        @param: tablero_usuario 
        @return correctos: total de piezas en su posicón correcta 
        */
        int totCorrectos(vector<vector<int>> tableroUsuario) {
            int correctos = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (tableroUsuario[i][j] == estadoFinal[i][j]) correctos += 10;
                }
            }
            return correctos;
        }

        /*
        Pedir y guardar los datos del usuario al final del juego 
        @param puntaje_acumulado: puntaje acumulado durante el juego 
        */
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
    
        /*
        Cambiar nivel 
        @param nivel_actual: nivel actual 
        @return nuevo nivel 
        */
        int cambiarNivel(int nivel_actual) {
            if (nivel_actual == 1) return 2; // Si el nivel es 1, cambiar a 2 
            return 1;                        // Si el nivel es 2, cambiar a 1
        }

        /* 
        Sugerir al usuario qué pieza mover 
        @param tablero: estado actual del tablero 
        @return numero de la pieza a mover 
        */
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

        /*
        Contar las piezas que están en su posición correcta 
        @param tablero: estado actual del tablero 
        @return contador: total de piezas en su posición correcta  
        */
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

        /*
        Calcular la distancia de Manhattan de una pieza específica 
        @param num: número del que se calculará la distancia
        @param tablero: estado actual del tablero 
        return distancia de Manhattan calculada 
        */
        int calcularDistanciaManhattan (int num, vector<vector<int>> tablero) {
            pair<int,int> posActual, posObjetivo;

            // Encontrar posición actual y objetivo de Manhattan 
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

            // Calcular distancia de Manhattan 
            return abs(posActual.first - posObjetivo.first) + abs(posActual.second - posObjetivo.second);
        }

        //-----------OTRAS FUNCIONES-----------

        // Limpiar matrices de estado inicial y final (se reinician a 0)
        void limpiarMatrices() {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    estadoInicial[i][j] = 0;
                    estadoFinal[i][j] = 0;
                }
            }            
        }

        /*
        Ver especificaciones del modo usuario y del modo inteligente
        @param modo: 'i' para modo inteligente y 'u' para modo usuario 
        */  
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

        // Explicación general del juego 
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
    srand(time(NULL)); // Inicialización de aleatorios 
    Puzzle p;
    p.mostrarMenuPrincipal(); // Mostrar menú 
    system("cls");
    cout << endl;
    // Mensaje de despedida 
    cout << " --------------------------\n";
    cout << "|         GRACIAS          |\n";
    cout << "|           POR            |\n";
    cout << "|          JUGAR           |\n"; 
    cout << " --------------------------";           
    return 0;
}