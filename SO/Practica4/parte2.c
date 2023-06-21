#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


int leds;

void operacioncircular(char aux[]);
void encender(int o);


int main(int argc, char *argv[]){
	leds = open("/dev/leds", O_RDWR);
    if(leds == -1) 
        return -1;

    char aux[200] ;
    scanf("%[^\n]", aux);
  
    operacioncircular(aux);

    close(leds);
    return 0;
}

void operacioncircular(char aux[]){
    /*
    Se sumaran las entradas de la cadena de caracteres.
    Si la suma total es un numero par, se encenderan los leds hacia la derecha.
    Si la suma total es impar, se encenderan los leds circularmente de derecha a izquierda.
    */

    int stotal = 0;
    bool apagados = false;
    for(int i = 0;i <strlen(aux) ; i++){
        int c = (int) aux[i];
        stotal += c;
    }
    int o;
    if(stotal%2 == 0) o = 1;
    else o = 2;
    encender(o);
    
}

void encender(int o){
    if(o == 1){ // par, por lo que hacia la derecha
        write(leds,"1",1);
        sleep(1);
        write(leds,"2",1);
        sleep(1);
        write(leds,"3",1);
        sleep(1);
    }
    else{
        write(leds,"3",1);
        sleep(1);
        write(leds,"2",1);
        sleep(1);
        write(leds,"1",1);
        sleep(1);
    }
}