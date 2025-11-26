#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h> // Para capturar teclas 
#include <fstream>
#include <vector>

using namespace std;

#define TAM 500

//-----------CLASE PARA MANEJO DE PUNTAJES (GUARDAR Y MOSTRAR EN EL ARCHIVO)-----------
class Clase_Usuario { 
    private:
        // Datos para guardar en el archivo
        struct Datos_Usuario {
            char nombreUsuario[TAM];
            int puntuacion;         // El puntaje debe ser acumulativo 
        };
        
    public:
        Clase_Usuario(){}
        // Guardar nombre del usuario que terminó su juego y su puntaje 
        void guardarNombrePuntaje(char nombre[TAM], int puntaje) {
            Datos_Usuario nuevo;

            // Si es el nombre ya existe se suman los puntajes (acumulación de puntaje)
            
        }

        // Ver los nombres de los usuarios y sus puntajes  
        void mostrarPuntajes() {
            Datos_Usuario guardado;
        }

};

//-----------CLASE PARA RESOLVER CON A* USANDO HUERÍSTICA DE MANHATAN-----------
class resolver_con_A {

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

            int nivel = 1;
            do{
                cout << "\n        PUZZLE 8         \n\n";
                cout << "\n------MODO INTELIGENTE-----:";
                cout << "\n0- Salir";
                cout << "\n1- Ver especificaciones del modo inteligente";
                cout << "\n2- Iniciar juego"; 
                cout << "\n3- Elegir nivel (nivel actual: " << nivel << ")"; 
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
                    case 3: nivel = cambiarNivel(nivel);
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
            resolver_con_A a;

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

            do{
                cout << "\n        PUZZLE 8         \n\n";
                cout << "\n------MODO USUARIO-----:";
                cout << "\n0- Salir";
                cout << "\n1- Ver especificaciones del modo usuario";
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
                    case 2: jugarModoUsuario(); break;
                    case 0: cout<<"Saliendo...";
                    default: cout<<"Opcion invalida\n";
                }

                if(opcion == 2) limpiarMatrices();

            }while(opcion!=0);            
        }

        void jugarModoUsuario() {
            // Gneración de un tablero aleatorio con números del 0 al 8, sin que se repitan números
            generarNuevoJuego(); 

            // Tablero del usuario
            vector<vector<int>> tableroUsuario;

            // Copiar estado inicial en el tablero 

            // Inicializar tablero como una matriz de 3*3
            inicializarTablero(tableroUsuario);

            // Resolver 
            resolver_con_A a;

            // Pedir datos del usuario una vez fianlizado el juego (Fin del juego es cuando el usaurio encuntra la solución o cuando decide salir)
            pedirDatosUsuario();

            // Mostrar los puntajes de todos los jugadores 
            Clase_Usuario us;
            us.mostrarPuntajes();
        }

        // Gneración de un tablero aleatorio con números del 0 al 8, sin que se repitan números
        void generarNuevoJuego() {
            // Generar estado inicial 

            // Generar estado final
            
        }

        void pedirDatosUsuario() {
            system("cls");
            char nombre[TAM];
            float punt;
            int puntaje;
            bool puntajeValido = false;
            Clase_Usuario us; 
            
            do{
                cout << "\n-----FIN DEL JUEGO-----\n";
                cout << "Nombre: "; 
                cin.ignore();
                cin.getline(nombre,TAM);
                cout << "Puntaje: "; cin >> punt;
                
                // Verificar que puntaje sea un número entero
		        if (cin.fail()){ // Si la entrada no es un numero
			        cin.clear(); // Limpiar estado de error de cin
			        cin.ignore(1000,'\n'); // Descartar la entrada incorrecta hasta mil caracteres o hasta encontrar un salto de linea
                    cout << "ERROR! puntaje no valido\n";
                    system("pause");
                    system("cls");
                }
		        else if (fmod(punt,1)!=0) { // Descartar numeros con decimales
                    cout << "ERROR! puntaje no valido\n";
                    system("pause");  
                    system("cls");                 
                }
		        else {
                    puntaje=static_cast<int>(punt); // Convertir entrada a enteros si es válida
                    puntajeValido = true;
                    system("cls");
                }

                
            }while(!puntajeValido);

            us.guardarNombrePuntaje(nombre, puntaje);
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