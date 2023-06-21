#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define N_PARADAS 5 // número de paradas de la ruta
#define EN_RUTA 0 // autobús en ruta
#define EN_PARADA 1 // autobús en la parada
#define MAX_USUARIOS 40 // capacidad del autobús
#define USUARIOS 4 // numero de usuarios
// estado inicial
int estado = EN_RUTA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0; // ocupantes que tiene el autobús
// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,...0};
// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0};
// Otras definiciones globales (comunicación y sincronización)
pthread_cond_t continua;
pthread_mutex_t m;


void * thread_autobus(void * args) {
	while (1) {
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();
		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}
void * thread_usuario(void * arg) {
	int id_usuario = (int) arg;
	int a, b;
	// obtener el id del usario
	while (1) {
		a = rand() % N_PARADAS;
		do{
			b = rand() % N_PARADAS;
		} while(a == b);
		Usuario(id_usuario, a, b);
	}
}
void Usuario(int id_usuario, int origen, int destino) {
	// Esperar a que el autobus esté en parada origen para subir
	Subir_Autobus(id_usuario, origen);
	// Bajarme en estación destino
	Bajar_Autobus(id_usuario, destino);
}
void Autobus_En_Parada(){
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Después se pone en marcha */

	pthread_mutex_lock(&m);
	estado = EN_PARADA;
	printf("El autobus (parado) en la parada: %d \n", parada_actual);
	printf("Esperando parada: %d \nEsperando para bajar: %d \n", esperando_parada[parada_actual], esperando_bajar[parada_actual]);
	while(esperando_bajar[parada_actual] != 0 || esperando_parada[parada_actual] != 0){
		pthread_cond_wait(&continua, &m);
	}	
	estado = EN_RUTA;
	printf("Bus en ruta\n");
	pthread_mutex_unlock(&m);
}
void Conducir_Hasta_Siguiente_Parada(){
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	sleep(12);
	pthread_mutex_lock(&m);

	if (parada_actual == N_PARADAS) {
		parada_actual = 0;
	}
	else {
		parada_actual++;
	}

	pthread_mutex_unlock(&m);
}
void Subir_Autobus(int id_usuario, int origen){
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
	el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	pthread_mutex_lock(&m);
	esperando_parada[origen]++;
	printf("El usuario %d esta esperando para subir en la parada: %d \n", id_usuario, origen);

	while(estado == EN_RUTA || parada_actual != origen){

		pthread_cond_wait(&continua, &m);
	} // hasta que lleguen wait
	esperando_parada[origen]--;
	n_ocupantes++; // aumentan ocupantes y disminuyen los que esperan
	printf("Sube usuario %d en la parada: %d \n", id_usuario, parada_actual);
	if (esperando_parada[origen] == 0) {
		pthread_cond_signal(&continua); // Mandar señal para continuar a siguiente parada
	}
	pthread_mutex_unlock(&m);
}
void Bajar_Autobus(int id_usuario, int destino){
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
	el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	pthread_mutex_lock(&m);
	esperando_bajar[destino]++;
	printf("Usuario %d esta esperando para bajar en la parada: %d \n", id_usuario, destino);
	while(parada_actual != destino || estado == EN_RUTA){
		pthread_cond_wait(&continua, &m);

	}
	
	n_ocupantes--;
	esperando_bajar[destino]--;

	printf("Baja el usuario %d en la parada: %d \n", id_usuario, parada_actual);
	if (esperando_bajar[destino] == 0) {
		pthread_cond_signal(&continua);
	}
	pthread_mutex_unlock(&m);
}

int main(int argc, char *argv[]) {
	int i;
	// Definición de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas
	// Crear el thread Autobus
	pthread_t usuarios [USUARIOS];
	pthread_t bus;

	pthread_mutex_init(&m, NULL);
	pthread_create(&bus, NULL, thread_autobus, (void*)0);
	pthread_cond_init(&continua, NULL);
	for (i = 0; i < USUARIOS; i++){
		// Crear thread para el usuario i
		pthread_create(&usuarios[i], NULL, thread_usuario, (void*)i);
		// Esperar terminación de los hilos
	}


	pthread_mutex_destroy(&m);

    pthread_cond_destroy(&continua);
	pthread_join(bus, NULL);
	for (i = 0; i < USUARIOS; i++) {
		pthread_join(usuarios[i], NULL);
	}
	return 0;
}

