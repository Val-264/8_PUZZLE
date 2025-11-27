#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <cctype>
#include <windows.h>
#include <conio.h> // Para capturar teclas 
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#define TAM 500
const int tiempo_espera = 1000; // Esperar un segundo antes de mostrar el sigueinte paso 

//-----------CLASE PARA MANEJO DE PUNTAJES (GUARDAR Y MOSTRAR EN EL ARCHIVO)-----------
class Clase_Usuario { 
    private:
        // Datos para guardar en el archivo
        struct Datos_Usuario {
            char nombreUsuario[TAM];
            int puntuacion;         // El puntaje debe ser acumulativo
            struct Fecha {
                int dd;
                int mm;
                int aa;
            }; 
            Fecha dia;
        };
        
    public:
        Clase_Usuario(){}
        
        // Guardar nombre del usuario que terminó su juego y su puntaje 
        void guardarNombrePuntaje(char nombre[TAM], int puntaje) {
            Datos_Usuario nuevo, existente;
            fstream puntajes;
            bool existe = false;
            streampos posicion;

            // Abrir archivo para lectoescritura
            puntajes.open("puntajes.dat", ios::binary | ios::in | ios::out);

            // Si el archivo no existe, crearlo
            if(!puntajes) {
                puntajes.open("puntajes.dat", ios::binary | ios::out);
                puntajes.close();
                puntajes.open("puntajes.dat", ios::binary | ios::in | ios::out);
                
                if(!puntajes) {
                    cerr << "\nNo se pudo abrir el archivo para guardar puntajes\n";
                    system("pause");
                    system("cls");
                    return;
                }
            }

            // Buscar si el usuario ya existe
            while (puntajes.read(reinterpret_cast<char*>(&existente), sizeof(Datos_Usuario))) {
                if (strcmp(existente.nombreUsuario, nombre) == 0) {
                    existe = true;
                    posicion = puntajes.tellg() - streampos(sizeof(Datos_Usuario));
                    break;
                }
            }

            if (existe) {
                // Actualizar usuario existente
                existente.puntuacion += puntaje;
                
                // Actualizar fecha
                time_t hoy = time(0);
                tm * timeinfo = localtime(&hoy);
                existente.dia.dd = timeinfo->tm_mday; 
                existente.dia.mm = timeinfo->tm_mon + 1; 
                existente.dia.aa = 1900 + timeinfo->tm_year;
                
                // Posicionarse y actualizar
                puntajes.seekp(posicion);
                puntajes.write(reinterpret_cast<char*>(&existente), sizeof(Datos_Usuario));
            } else {
                // Crear nuevo usuario
                strcpy(nuevo.nombreUsuario, nombre);
                nuevo.puntuacion = puntaje;

                // Obtener fecha actual CORREGIDA
                time_t hoy = time(0);
                tm * timeinfo = localtime(&hoy);
                nuevo.dia.dd = timeinfo->tm_mday;    // Día del mes (1-31)
                nuevo.dia.mm = timeinfo->tm_mon + 1; // Mes (1-12)
                nuevo.dia.aa = 1900 + timeinfo->tm_year;
                
                // Escribir al final del archivo
                puntajes.clear(); // Limpiar flags de error
                puntajes.seekp(0, ios::end);
                puntajes.write(reinterpret_cast<char*>(&nuevo), sizeof(Datos_Usuario));
            }

            puntajes.close();
        }

        // Ver los nombres de los usuarios y sus puntajes  
        void mostrarPuntajes() {
            Datos_Usuario guardado;
            ifstream puntajes; // Usar ifstream para solo lectura

            // Abrir archivo para lectura
            puntajes.open("puntajes.dat", ios::binary | ios::in);

            // Verificar si el archivo existe y tiene datos
            if (!puntajes) {
                cout << "\nNo hay puntajes guardados aún.\n";
                system("pause");
                system("cls");
                return;
            }

            // Verificar si el archivo está vacío
            puntajes.seekg(0, ios::end);
            if (puntajes.tellg() == 0) {
                cout << "\nNo hay puntajes guardados aún.\n";
                puntajes.close();
                system("pause");
                system("cls");
                return;
            }
            puntajes.seekg(0, ios::beg); // Volver al inicio

            // Mostrar archivo 
            cout << "\n                PUZZLE 8         \n\n";
            cout << "------------------PUNTAJES------------------\n";
            cout << "Nombre\t\tPuntaje\tFecha\n";
            cout << "--------------------------------------------\n";

            // Leer y mostrar todos los registros
            while (puntajes.read(reinterpret_cast<char*>(&guardado), sizeof(Datos_Usuario))) {
                cout << guardado.nombreUsuario << "\t\t" 
                     << guardado.puntuacion << "\t"  
                     << guardado.dia.dd << "/" << guardado.dia.mm << "/" << guardado.dia.aa << endl;
            }

            puntajes.close();
            cout << "\n--------------------------------------------\n";
            system("pause");
            system("cls");
        }
};

//-----------CLASE PARA RESOLVER CON A* USANDO HUERÍSTICA DE MANHATAN-----------
class A_star {

    private:
};

//-----------CLASE DE CONTROL DEL JUEGO-----------
class Puzzle{
    private:
        // Estas matrices son usadas por ambos modos de juego 
        vector<vector<int>> estadoInicial;
        vector<vector<int>> estadoFinal;
    public:
        // Inicializar variables
        Puzzle(){
            // Incializar matrices (3*3)
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

                // Verificar entrada válida
		        if (cin.fail()){ // Si la entrada no es un numero
			        cin.clear(); // Limpiar estado de error de cin
			        cin.ignore(1000,'\n'); // Descartar la entrada incorrecta hasta mil caracteres o hasta encontrar un salto de linea
                    opcion=500;
                }
		        else if (fmod(opc,1)!=0) opcion=500; // Descartar numeros con decimales
		        else opcion=static_cast<int>(opc); // Convertir entrada a enteros si es válida

                system("cls");

                switch (opcion){
                    case 1: menuModoInteligente(); break;
                    case 2: menuModoUsuario(); break;
                    case 3: explicarJuego(); break;
                    case 4: us.mostrarPuntajes(); break;
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                // Limpiar matrices de estado inical y final antes de un nuevo juego
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
                cout << "\n        PUZZLE 8         \n\n";
                cout << "\n------MODO INTELIGENTE-----:";
                cout << "\n0- Salir";
                cout << "\n1- Ver especificaciones del modo inteligente";
                cout << "\n2- Iniciar juego";  
                cout << "\nElige una opcion: ";
                cin >> opc;

                // Verificar entrada válida
		        if (cin.fail()){ // Si la entrada no es un numero
			        cin.clear(); // Limpiar estado de error de cin
			        cin.ignore(1000,'\n'); // Descartar la entrada incorrecta hasta mil caracteres o hasta encontrar un salto de linea
                    opcion=500;
                }
		        else if (fmod(opc,1)!=0) opcion=500; // Descartar numeros con decimales
		        else opcion=static_cast<int>(opc); // Convertir entrada a enteros si es válida

                system("cls");

                switch (opcion){
                    case 1: verEspecificaciones('i'); break;
                    case 2: jugarModoInteligente(); break;
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                // Limpiar matrices de estado inical y final antes de un nuevo juego
                if(opcion == 2 || opcion == 0) limpiarMatrices();

                system("cls");
            }while(opcion!=0);                        
        }

        void jugarModoInteligente () {
            capturarInicioFin();

            // Tablero para el juego
            vector<vector<int>> tableroInteligente;

            // Copiar estado inicial en el tablero 

            // Inicializar tablero como una matriz de 3*3
            inicializarTablero(tableroInteligente);

            // Resolver 
            A_star a;

            cout << "\n-----FIN DEL JUEGO-----\n";

        }

        void capturarInicioFin() {
            bool pantallaLimpia = false;
            bool matrizValida = false;

            cout << "\n        PUZZLE 8         \n\n";
            cout << "\n------MODO INTELIGENTE-----";
            cout << "\nIngresa los numeros para el estado inicial y final (solucion) del juego";
            cout << "\nDeben ser numeros entre 0 y 8 y ninguno debe repetirse\n";

            // Capturar estado inical del puzzle
            do{
                // Volver imprimir encabezado si se limpio la pantalla porque la entrada fue invalida
                if (pantallaLimpia) {
                    cout << "\n        PUZZLE 8         \n\n";
                    cout << "\n------MODO INTELIGENTE-----";
                    cout << "\nIngresa los numeros para el estado inicial y final (solucion) del juego";
                    cout << "\nDeben ser numeros entre 0 y 8 y ninguno debe repetirse\n";
                }

                cout << "\nEstado incial del puzzle\n";
                for(int i = 0; i < 3; i ++) {
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

            // Capturar estado final (meta) del puzzle 
            matrizValida = false;
            do{
                // Volver imprimir la información que estaba nantes de limpiar la pantalla
                if (pantallaLimpia) {
                    cout << "\n        PUZZLE 8         \n\n";
                    cout << "\n------MODO INTELIGENTE-----";
                    cout << "\nIngresa los numeros para el estado inicial y final (solución) del juego";
                    cout << "\nDeben ser numeros entre 0 y 8 y ninguno debe repetirse\n";
                    cout << "\nEstado incial del puzzle\n";
                    for(int i = 0; i < 3; i ++) {
                        for (int j = 0; j < 3; j++) {
                            cout << estadoInicial[i][j] << " ";
                            if (j == 2) cout << endl;
                        }
                    }
                }

                cout << "\nEstado final (meta) del puzzle\n";
                for(int i = 0; i < 3; i ++) {
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



        // Verificar que las matrices de estado inicual y final solo contengan números permitidos 
        bool verificarEspacios(vector<vector<int>> matriz) {
            vector<int> existentes; // Vector de los  números que ya están en la matriz 
            int revisados = 0;   // Total de números que ya se revisaron 

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    // Verificar las condiciones para los campos de la matriz, si no se cumplen retornar 0 
                    if (matriz[i][j] < 0||  matriz[i][j] > 8) {
                        cout << "\nHay numeros fuera de rango: los numeros deben estar entre 0 y 8\n";
                        return false;
                    }
                    
                    // Revisar que no haya repetidos (a partir del segundo número ya puede haber repeticiones)
                    if (revisados > 0) {
                        for (int k = 0; k < revisados; k++) {
                            if (matriz[i][j] == existentes[k]) {
                                cout << "\nHay numeros repetidos: "<< matriz[i][j] << "\n";
                                return false;
                            } 
                        }
                    }

                    // Si no se repitió, guardar en el vector de existentes 
                    existentes.emplace_back(matriz[i][j]);
                    revisados++;
                    
                }
            }

            // Si la matriz no tiene errores, retornar verdadero 
            return true;
        }

        //-----------FUNCIONES PARA MODO USUARIO-----------
        void menuModoUsuario(){
            float opc;
            int opcion;
            int nivel = 1;

            do{
                cout << "\n        PUZZLE 8         \n\n";
                cout << "\n------MODO USUARIO-----:";
                cout << "\n0- Salir";
                cout << "\n1- Ver especificaciones del modo usuario";
                cout << "\n3- Elegir nivel (nivel actual: " << nivel << ")";
                cout << "\n2- Iniciar juego"; 
                cout << "\nElige una opcion: ";
                cin >> opc;

                // Verificar entrada válida
		        if (cin.fail()){ // Si la entrada no es un numero
			        cin.clear(); // Limpiar estado de error de cin
			        cin.ignore(1000,'\n'); // Descartar la entrada incorrecta hasta mil caracteres o hasta encontrar un salto de linea
                    opcion=500;
                }
		        else if (fmod(opc,1)!=0) opcion=500; // Descartar numeros con decimales
		        else opcion=static_cast<int>(opc); // Convertir entrada a enteros si es válida

                system("cls");

                switch (opcion){
                    case 1: verEspecificaciones('u'); break;
                    case 2: jugarModoUsuario(nivel); break;
                    case 3: nivel = cambiarNivel(nivel);
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                if(opcion == 2) limpiarMatrices();

            }while(opcion!=0);            
        }

        void jugarModoUsuario(int nivel) {
            // Gneración de un tablero aleatorio con números del 0 al 8, sin que se repitan números
            generarNuevoJuego(nivel); 

            // Tablero del usuario
            vector<vector<int>> tableroUsuario;

            // Copiar estado inicial en el tablero 

            // Inicializar tablero como una matriz de 3*3
            inicializarTablero(tableroUsuario);

            // Resolver 
            int puntaje_acumulado = 0;
            A_star a;

            // Pedir datos del usuario una vez fianlizado el juego (Fin del juego es cuando el usaurio encuntra la solución o cuando decide salir)
            pedirDatosUsuario(puntaje_acumulado);

            // Mostrar los puntajes de todos los jugadores 
            Clase_Usuario us;
            us.mostrarPuntajes();
        }

        // Gneración de un tablero aleatorio con números del 0 al 8, sin que se repitan números
        void generarNuevoJuego(int nivel) {
            // El estado final siempre es el mismo: 0, 1, 2, 3, 4, 5, 6, 7, 8 
            int numeros_validos = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    estadoFinal[i][j] == numeros_validos;
                    numeros_validos++;
                }
            }

            // Generar estado inicial de forma aleatoira: 
            //nivel 1 = menos movimientos para llegar a estado final, nivel 2 = más movimientos para llegar a estado final
            
        }

        // 
        int totCorrectos(vector<vector<int>> tableroUsuario) {
            int correctos = 0;

            // Verificar cuántas piezas están en el lugar correcto cada que se mueve una pieza para mostrar el puntaje al usuario durante el juego
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (tableroUsuario[i][j] == estadoFinal[i][j]) correctos++;
                    else correctos--;
                }
            }

            return correctos;
        }

        void pedirDatosUsuario(int puntaje_acumulado) {
            system("cls");
            char nombre[TAM];
            Clase_Usuario us; 
            
            cout << "\n-----FIN DEL JUEGO-----";
            cout << "\nLo hisciste muy bien!, cual es tu nombre?\n";
            cout << "Nombre: "; 
            cin.ignore();
            cin.getline(nombre,TAM);

            us.guardarNombrePuntaje(nombre, puntaje_acumulado);
        }
    
        //-----------OTRAS FUNCIONES-----------
        
        int cambiarNivel(int nivel_actual) {
            if (nivel_actual == 1) return 2; // Si el nivel actual es 1, cambiar a 2
            return 1; // Si el nivel actual es 2, cambiar a 1
        }

        void limpiarMatrices() {
            // Limpiar matrices de estado inical y final antes de un nuevo juego
            for (int i = 0; i < 3 ; i++) {
                for (int j = 0; j <3; j++) {
                    estadoInicial[i][j] = 0;
                    estadoFinal[i][j] = 0;
                }
            }            
        }

        // Inicializar tableros comomatrices de 3*3
        void inicializarTablero(vector<vector<int>> tablero) {
            tablero.resize(3, vector<int>(3,0));
        }

        void verEspecificaciones(char modo) { // Si modo == 'u' se meustran las especificaiones de modo usuario, si modo == 'i' se muestran las especificaciones de modo inteligente 
            cout << "\n------ESPECIFICACIONES ";

            if (modo == 'u') {
                cout << "MODO USUARIO------\n";
            }

            else { // Si no es u es i 
                cout << "MODO INTELIGENTE------\n";
            }

            system("pause");
            system("cls");
        }

        void explicarJuego() {
            cout << "\n------PUZZLE 8-----\n";

            system("pause");
            system("cls");
        }

};

int main () {
    srand(time(NULL)); // Inicializar generación de aleatorios 

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