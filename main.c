#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "respuestas.h"
#include "structures.h"
#include <stdlib.h>
#include "cadenas.h"
#include "filesystem.h"

void menu();
void chooseCommand(char * command);
void ejecutarScript(char ** comando);
void vaciarLista(IDLIST *lista);

//variables globales
IDLIST lista;

int main()
{
    vaciarLista(&lista);
    menu();
    return 0;
}

void menu()
{
    printf("eXTvolucion: ");

    //obtengo el comando aqui
    char buffer[256] = { 0 };
    fgets(buffer, sizeof(buffer), stdin);
    int size = strlen(buffer);
    char * comando = (char*)malloc((size*sizeof(char)) + 1);
    memset(comando, '\0', size);
    comando = strcpy(comando, buffer);


    //aqui lo preparo para el parseo
    comando = prepararCadena(comando);

    //aqui lo parseo y analizo
    if (*comando == 0)
        printf("Ha ingresado un comentario\n");
    else if (strcmp(comando, "clear") == 0) {
        system("clear");
    }
    else if (strcmp(comando, "exit") == 0) {
        printf("Hasta luego\n");
        exit(1);
    }
    else {
        chooseCommand(comando);
    }

    //limpiamos las cosas que usamos, por si acaso
    memset(comando, '\0', size);
    if (comando) {
        free(comando);
        comando = 0;
    }
    for (int i = 0; i < 256; i++)
        buffer[i] = 0;

    menu();
}

void chooseCommand(char *command) {
    if (command == 0 || *command == 0) return;

    char** comando = split_string(command, " ");

    if (!(strcmp(*comando, "mkdisk"))) {
        int response = makeDisk(++comando);
        respuestasMkDisk(response);
    }
    else if (!strcmp(*comando, "rmdisk")) {
        int response = rmDisk(++comando);
        respuestasRmDisk(response);
    }
    else if (!strcmp(*comando, "fdisk")) {
        int response = fDisk(++comando);
        respuestasFDisk(response);
    }
    else if (!strcmp(*comando, "mount")) {
        int response = mount(++comando, &lista);
        respuestasMount(response);
    }
    else if (!strcmp(*comando, "unmount")) {
        int response = unmount(++comando, &lista);
        respuestasUnmount(response);
    }
    else if (!strcmp(*comando, "rep")) {
        int response = report(++comando);
        respuestasReport(response);
    }
    else if (!strcmp(*comando, "exec")) {
        ejecutarScript(++comando);
    }
    else if (!strcmp(*comando, "mkfs")) {

    }
    else if (!strcmp(*comando, "login")) {

    }
    else if (!strcmp(*comando, "logout")) {

    }
    else if (!strcmp(*comando, "mkgrp")) {

    }
    else if (!strcmp(*comando, "rmgrp")) {

    }
    else if (!strcmp(*comando, "mkusr")) {

    }
    else if (!strcmp(*comando, "rmusr")) {

    }
    else if (!strcmp(*comando, "chmod")) {

    }
    else if (!strcmp(*comando, "mkfile")) {

    }
    else if (!strcmp(*comando, "cat")) {

    }
    else if (!strcmp(*comando, "rem")) {

    }
    else if (!strcmp(*comando, "edit")) {

    }
    else if (!strcmp(*comando, "ren")) {

    }
    else if (!strcmp(*comando, "mkdir")) {

    }
    else if (!strcmp(*comando, "cp")) {

    }
    else if (!strcmp(*comando, "mv")) {

    }
    else if (!strcmp(*comando, "find")) {

    }
    else if (!strcmp(*comando, "chown")) {

    }
    else if (!strcmp(*comando, "chgrp")) {

    }
    else if (!strcmp(*comando, "pause")) {

    }
    else if (!strcmp(*comando, "convert")) {

    }
    else if (!strcmp(*comando, "recovery")) {

    }
    else if (!strcmp(*comando, "loss")) {

    }
    else
        printf("El comando ingresado no existe\n");
    printf("\n");
}

void ejecutarScript(char ** comando) {
    //obtenemos el path
    char * path = (char*) malloc((256) * sizeof(char));
    memset(path, '\0', 256);

    char ** aux = comando;
    char ** direccion = split_string(*aux, "~:~");
    if (strcmp(*direccion, "-path") != 0 && strcmp(*direccion, "path")) {
        printf("El unico parametro del comando exec es path\n"); //el unico parametro del comando exec es path
        return;
    }

    direccion++;
    strcpy(path, *direccion);
    if (path[0] == 0) {
        printf("No ingreso una direccion valida\n");
        return;
    }
    reemplazar(path, 38, 32);
    doblesPorSimples(path);
    quitarComillas(path);

    if (fileExists(path) == 0) {
        printf("No existe el archivo representado por la direccion ingresada\n");
        return;
    }

    FILE * archivo;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    archivo = fopen(path, "r");
    if (archivo == NULL) {
        printf("La direccion existe pero no es un archivo");
        return;
    }

    while ((read = getline(&line, &len, archivo)) != -1) {
        //verificamos que no venga vacia
        if (!len) continue;
        if (!(strcmp(line, "")) || !(strcmp(line, "\n"))) continue;

        //metemos la cadena en un nuevo char *
        char * comando = (char*) malloc(512*sizeof(char));
        memset(comando, 0, 512);
        strcpy(comando, line);

        //analizamos la linea
        printf("%s\n", comando);
        comando = prepararCadena(comando);

        if (*comando == 0)
            printf("Ha ingresado un comentario\n");
        else if (strcmp(comando, "clear") == 0) {
            system("clear");
        }
        else if (strcmp(comando, "exit") == 0) {
            exit(1);
        }
        else {
            chooseCommand(comando);
        }

        //liberamos la memoria que usamos
        if (comando)
            free(comando);
        //if (line)
            //free(line);
    }

    fclose(archivo);
    if (line)
        free(line);

    printf("El archivo fue ejecutado con exito\n");
}

void vaciarLista(IDLIST *lista) {
    for(int i = 0; i < 400; i++) {
        memset(lista->ids[i].id, 0, 7);
    }
    lista->current = 'a';
    lista->active = 0;
    for (int j = 0; j < 26; j++) {
        lista->correlatives[j] = 1;
    }
}
