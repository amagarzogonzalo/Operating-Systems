#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int copynFile(FILE * origin, FILE * destination, int nBytes)
{
	int ini=0;
	int data = getc(origin);
	while(data != EOF && ini < nBytes ){
		putc(data,destination);
		data=getc(origin);
		ini++;
	}
	if(ini!= 0){
		return ini;
	}
	else return -1;
	
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	char* returned = malloc(sizeof(char*));
	char* cadena = malloc(sizeof(char));
	if(fread(cadena, sizeof(char),1,file)>0){
		while(strcmp(cadena, "\0")){
			strcat(returned, cadena); // pasa el valor de cadena a returned añade a lo que que ya haya
			fread(cadena, sizeof(char),1, file);
		}
		free(cadena);
		return returned;
	}
	return NULL;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	stHeaderEntry* matriz = NULL;
	int num = 0;
	if((fread(&num, sizeof(int),1,tarFile))>0){ //te devuelve el numero de elementos leidos 
		matriz = malloc(sizeof(stHeaderEntry)*num); //reservas espacio en memoria para la matriz de pares
		for(int i = 0; i < num; i++){
			matriz[i].name = loadstr(tarFile);//loadstr: lee una cadena de caracteres de tarFile
			fread(&matriz[i].size, sizeof(int),1,tarFile);
		}
		(*nFiles)=num;
		return matriz;
	}
	return NULL;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE* destino;
	destino = fopen(tarName, "w"); //inicializar el fichero (w crea el fichero en formato escritura) (tarName: abre el contenido de tarName que tiene que ser un cadena de caracteres)
	stHeaderEntry* matriz = malloc(sizeof( stHeaderEntry)* nFiles); // reserva espacio en memoria y free lo libera devuelve un putero
	int filespace = 0;
	for (int i = 0; i < nFiles; i++){
		matriz[i].name = malloc (strlen(fileNames[i])+1);
		strcat(matriz[i].name, fileNames[i]); //recibe dos punteros, agrega la cadena apuntada por filenames[i] a array[i].name
		filespace +=(strlen(fileNames[i])+1); //calcula lo que ocupa el total de cabeceras
	}
	long int desp = filespace + sizeof(int) + nFiles*sizeof(unsigned int); // segundo: N tamaño tercero: tamaños archivos
	fseek(destino, desp, SEEK_CUR); // devuelve un entero y recibe un puntero a un finchero un lon y un int: destino deja de apuntar al principio para apuntar a desplazamiento
	FILE* entrada;
	for (int i = 0; i < nFiles; i++){
		entrada = fopen(fileNames[i],"r");
		int aux = copynFile(entrada, destino, INT_MAX); 
		if(aux == -1){
			return EXIT_FAILURE;
		}
		fclose(entrada);
		desp+=aux;
		matriz[i].size = aux; //no sabes tamaño fichero hasta que copias datos :)
	}
	fseek(destino,-desp, SEEK_CUR);
	fwrite(&nFiles,sizeof(int),1,destino); //permite escribir nitems 
	for(int i =0; i < nFiles; i++){
		fwrite(matriz[i].name, sizeof(char)*strlen(matriz[i].name), 1, destino); //escribes la ruta de fichero
		putc('\0', destino); // la ruta acaba con \0 
		fwrite(&matriz[i].size, sizeof(int),1,destino);//escribes el tamaño del fichero
		
	}
	for(int i = 0; i < nFiles; i++){
			free(matriz[i].name);
		}
	
	fclose(destino);
	free(matriz);
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	FILE* tarfile;
	tarfile = fopen(tarName, "r");
	int *nFiles = malloc(sizeof(int));
	stHeaderEntry* matriz = readHeader(tarfile, nFiles);
	if(matriz != NULL){
		long int desp = sizeof(int)+(*nFiles)*sizeof(unsigned int);
		for(int i = 0; i < *nFiles; i++){
			desp += (strlen(matriz[i].name)+1);
		}
		int aux = 0;
		FILE* destino;
		for(int i = 0; i < (*nFiles); i++){
			destino = fopen(matriz[i].name, "w");
			desp += aux;
			fseek(tarfile, desp, SEEK_SET); //SEEK_SET TE DESPLAZAS DESP DESDE EL COMIENZO DEL FICHERO TARFILE
			aux = copynFile(tarfile, destino, matriz[i].size);
			fseek(tarfile, -desp, SEEK_CUR);
			fclose(destino);
			
		}
		for(int i = 0; i < *nFiles; i++){
			free(matriz[i].name);
		}
		free(matriz);
		free(nFiles);
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}
